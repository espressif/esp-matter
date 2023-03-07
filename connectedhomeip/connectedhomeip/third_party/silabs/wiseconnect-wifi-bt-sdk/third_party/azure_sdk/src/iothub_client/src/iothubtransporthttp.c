// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>
#include "azure_c_shared_utility/gballoc.h"

#include <time.h>
#include "iothub_client_options.h"
#include "iothub_client_version.h"
#include "iothub_transport_ll.h"
#include "iothubtransporthttp.h"
#include "internal/iothub_client_private.h"
#include "internal/iothubtransport.h"
#include "internal/iothub_transport_ll_private.h"
#include "internal/iothub_internal_consts.h"

#include "azure_c_shared_utility/optimize_size.h"
#include "azure_c_shared_utility/httpapiexsas.h"
#include "azure_c_shared_utility/urlencode.h"
#include "azure_c_shared_utility/xlogging.h"
#include "azure_c_shared_utility/httpapiex.h"
#include "azure_c_shared_utility/httpapiexsas.h"
#include "azure_c_shared_utility/strings.h"
#include "azure_c_shared_utility/azure_base64.h"
#include "azure_c_shared_utility/doublylinkedlist.h"
#include "azure_c_shared_utility/vector.h"
#include "azure_c_shared_utility/httpheaders.h"
#include "azure_c_shared_utility/agenttime.h"

#define IOTHUB_APP_PREFIX "iothub-app-"
static const char* IOTHUB_MESSAGE_ID = "iothub-messageid";
static const char* IOTHUB_CORRELATION_ID = "iothub-correlationid";
static const char* IOTHUB_CONTENT_TYPE_D2C = "iothub-contenttype";
static const char* IOTHUB_CONTENT_ENCODING_D2C = "iothub-contentencoding";
static const char* IOTHUB_CONTENT_TYPE_C2D = "ContentType";
static const char* IOTHUB_CONTENT_ENCODING_C2D = "ContentEncoding";

static const char* IOTHUB_AUTH_HEADER_VALUE = "Authorization";

#define CONTENT_TYPE "Content-Type"
#define APPLICATION_OCTET_STREAM "application/octet-stream"
#define APPLICATION_VND_MICROSOFT_IOTHUB_JSON "application/vnd.microsoft.iothub.json"
#define API_VERSION "?api-version=2016-11-14"
#define EVENT_ENDPOINT "/messages/events"
#define MESSAGE_ENDPOINT_HTTP "/messages/devicebound"
#define MESSAGE_ENDPOINT_HTTP_ETAG "/messages/devicebound/"

/*DEFAULT_GETMINIMUMPOLLINGTIME is the minimum time in seconds allowed between 2 consecutive GET issues to the service (GET=fetch messages)*/
/*the default is 25 minutes*/
#define DEFAULT_GETMINIMUMPOLLINGTIME ((unsigned int)25*60)

#define MAXIMUM_MESSAGE_SIZE (255*1024-1)
#define MAXIMUM_PAYLOAD_OVERHEAD 384
#define MAXIMUM_PROPERTY_OVERHEAD 16

/*forward declaration*/
static int appendMapToJSON(STRING_HANDLE existing, const char* const* keys, const char* const* values, size_t count);

typedef struct HTTPTRANSPORT_HANDLE_DATA_TAG
{
    STRING_HANDLE hostName;
    HTTPAPIEX_HANDLE httpApiExHandle;
    bool doBatchedTransfers;
    unsigned int getMinimumPollingTime;
    VECTOR_HANDLE perDeviceList;

    TRANSPORT_CALLBACKS_INFO transport_callbacks;
    void* transport_ctx;

}HTTPTRANSPORT_HANDLE_DATA;

typedef struct HTTPTRANSPORT_PERDEVICE_DATA_TAG
{
    HTTPTRANSPORT_HANDLE_DATA* transportHandle;

    STRING_HANDLE deviceId;
    STRING_HANDLE deviceKey;
    STRING_HANDLE deviceSasToken;
    STRING_HANDLE eventHTTPrelativePath;
    STRING_HANDLE messageHTTPrelativePath;
    HTTP_HEADERS_HANDLE eventHTTPrequestHeaders;
    HTTP_HEADERS_HANDLE messageHTTPrequestHeaders;
    STRING_HANDLE abandonHTTPrelativePathBegin;
    HTTPAPIEX_SAS_HANDLE sasObject;
    bool DoWork_PullMessage;
    time_t lastPollTime;
    bool isFirstPoll;

    void* device_transport_ctx;
    PDLIST_ENTRY waitingToSend;
    DLIST_ENTRY eventConfirmations; /*holds items for event confirmations*/
} HTTPTRANSPORT_PERDEVICE_DATA;

typedef struct MESSAGE_DISPOSITION_CONTEXT_TAG
{
    HTTPTRANSPORT_HANDLE_DATA* handleData;
    HTTPTRANSPORT_PERDEVICE_DATA* deviceData;
    char* etagValue;
} MESSAGE_DISPOSITION_CONTEXT;

MU_DEFINE_ENUM_STRINGS_WITHOUT_INVALID(IOTHUBMESSAGE_DISPOSITION_RESULT, IOTHUBMESSAGE_DISPOSITION_RESULT_VALUES);

static void destroy_eventHTTPrelativePath(HTTPTRANSPORT_PERDEVICE_DATA* handleData)
{
    STRING_delete(handleData->eventHTTPrelativePath);
    handleData->eventHTTPrelativePath = NULL;
}

static bool create_eventHTTPrelativePath(HTTPTRANSPORT_PERDEVICE_DATA* handleData, const char * deviceId)
{
    /*Codes_SRS_TRANSPORTMULTITHTTP_17_017: [ IoTHubTransportHttp_Register shall create an immutable string (further called "event HTTP relative path") from the following pieces: "/devices/" + URL_ENCODED(deviceId) + "/messages/events" + APIVERSION. ]*/
    bool result;
    STRING_HANDLE urlEncodedDeviceId;
    handleData->eventHTTPrelativePath = STRING_construct("/devices/");
    if (handleData->eventHTTPrelativePath == NULL)
    {
        LogError("STRING_construct failed.");
        result = false;
    }
    else
    {
        if (!(
            ((urlEncodedDeviceId = URL_EncodeString(deviceId)) != NULL) &&
            (STRING_concat_with_STRING(handleData->eventHTTPrelativePath, urlEncodedDeviceId) == 0) &&
            (STRING_concat(handleData->eventHTTPrelativePath, EVENT_ENDPOINT API_VERSION) == 0)
            ))
        {
            /*Codes_SRS_TRANSPORTMULTITHTTP_17_018: [ If creating the string fail for any reason then IoTHubTransportHttp_Register shall fail and return NULL. ]*/
            destroy_eventHTTPrelativePath(handleData);
            LogError("Creating HTTP event relative path failed.");
            result = false;
        }
        else
        {
            result = true;
        }
        STRING_delete(urlEncodedDeviceId);
    }
    return result;
}

static int retrieve_message_properties(HTTP_HEADERS_HANDLE resp_header, IOTHUB_MESSAGE_HANDLE recv_msg)
{
    int result = 0;
    /*Codes_SRS_TRANSPORTMULTITHTTP_17_090: [All the HTTP headers of the form iothub-app-name:somecontent shall be transformed in message properties {name, somecontent}.]*/
    /*Codes_SRS_TRANSPORTMULTITHTTP_17_091: [The HTTP header of iothub-messageid shall be set in the MessageId.]*/
    size_t nHeaders;
    if (HTTPHeaders_GetHeaderCount(resp_header, &nHeaders) != HTTP_HEADERS_OK)
    {
        LogError("unable to get the count of HTTP headers");
        result = MU_FAILURE;
    }
    else
    {
        MAP_HANDLE properties = (nHeaders > 0) ? IoTHubMessage_Properties(recv_msg) : NULL;
        for (size_t index = 0; index < nHeaders; index++)
        {
            char* completeHeader;
            if (HTTPHeaders_GetHeader(resp_header, index, &completeHeader) != HTTP_HEADERS_OK)
            {
                LogError("Failed getting header %lu", (unsigned long)index);
                result = MU_FAILURE;
                break;
            }
            else
            {
                char* colon_pos;
                if (strncmp(IOTHUB_APP_PREFIX, completeHeader, strlen(IOTHUB_APP_PREFIX)) == 0)
                {
                    /*looks like a property headers*/
                    /*there's a guaranteed ':' in the completeHeader, by HTTP_HEADERS module*/
                    colon_pos = strchr(completeHeader, ':');
                    if (colon_pos != NULL)
                    {
                        *colon_pos = '\0'; /*cut it down*/
                        if (Map_AddOrUpdate(properties, completeHeader + strlen(IOTHUB_APP_PREFIX), colon_pos + 2) != MAP_OK) /*whereIsColon+1 is a space because HTTPEHADERS outputs a ": " between name and value*/
                        {
                            free(completeHeader);
                            result = MU_FAILURE;
                            break;
                        }
                    }
                }
                else if (strncmp(IOTHUB_MESSAGE_ID, completeHeader, strlen(IOTHUB_MESSAGE_ID)) == 0)
                {
                    char* whereIsColon = strchr(completeHeader, ':');
                    if (whereIsColon != NULL)
                    {
                        *whereIsColon = '\0'; /*cut it down*/
                        if (IoTHubMessage_SetMessageId(recv_msg, whereIsColon + 2) != IOTHUB_MESSAGE_OK)
                        {
                            free(completeHeader);
                            result = MU_FAILURE;
                            break;
                        }
                    }
                }
                else if (strncmp(IOTHUB_CORRELATION_ID, completeHeader, strlen(IOTHUB_CORRELATION_ID)) == 0)
                {
                    char* whereIsColon = strchr(completeHeader, ':');
                    if (whereIsColon != NULL)
                    {
                        *whereIsColon = '\0'; /*cut it down*/
                        if (IoTHubMessage_SetCorrelationId(recv_msg, whereIsColon + 2) != IOTHUB_MESSAGE_OK)
                        {
                            free(completeHeader);
                            result = MU_FAILURE;
                            break;
                        }
                    }
                }
                // Codes_SRS_TRANSPORTMULTITHTTP_09_003: [ The HTTP header value of `ContentType` shall be set in the `IoTHubMessage_SetContentTypeSystemProperty`. ]
                else if (strncmp(IOTHUB_CONTENT_TYPE_C2D, completeHeader, strlen(IOTHUB_CONTENT_TYPE_C2D)) == 0)
                {
                    char* whereIsColon = strchr(completeHeader, ':');
                    if (whereIsColon != NULL)
                    {
                        *whereIsColon = '\0'; /*cut it down*/
                        if (IoTHubMessage_SetContentTypeSystemProperty(recv_msg, whereIsColon + 2) != IOTHUB_MESSAGE_OK)
                        {
                            LogError("Failed setting IoTHubMessage content-type");
                            result = MU_FAILURE;
                            break;
                        }
                    }
                }
                // Codes_SRS_TRANSPORTMULTITHTTP_09_004: [ The HTTP header value of `ContentEncoding` shall be set in the `IoTHub_SetContentEncoding`. ]
                else if (strncmp(IOTHUB_CONTENT_ENCODING_C2D, completeHeader, strlen(IOTHUB_CONTENT_ENCODING_C2D)) == 0)
                {
                    char* whereIsColon = strchr(completeHeader, ':');
                    if (whereIsColon != NULL)
                    {
                        *whereIsColon = '\0'; /*cut it down*/
                        if (IoTHubMessage_SetContentEncodingSystemProperty(recv_msg, whereIsColon + 2) != IOTHUB_MESSAGE_OK)
                        {
                            LogError("Failed setting IoTHubMessage content-encoding");
                            break;
                        }
                    }
                }
                free(completeHeader);
            }
        }
    }
    return result;
}

static int set_system_properties(IOTHUB_MESSAGE_LIST* message, HTTP_HEADERS_HANDLE headers)
{
    int result;
    const char* msgId;
    const char* corrId;
    const char* content_type;
    const char* contentEncoding;

    // Add the Message Id and the Correlation Id
    msgId = IoTHubMessage_GetMessageId(message->messageHandle);
    if (msgId != NULL && HTTPHeaders_ReplaceHeaderNameValuePair(headers, IOTHUB_MESSAGE_ID, msgId) != HTTP_HEADERS_OK)
    {
        LogError("unable to HTTPHeaders_ReplaceHeaderNameValuePair");
        result = MU_FAILURE;
    }
    else if ((corrId = IoTHubMessage_GetCorrelationId(message->messageHandle)) != NULL && HTTPHeaders_ReplaceHeaderNameValuePair(headers, IOTHUB_CORRELATION_ID, corrId) != HTTP_HEADERS_OK)
    {
        LogError("unable to HTTPHeaders_ReplaceHeaderNameValuePair");
        result = MU_FAILURE;
    }
    // Codes_SRS_TRANSPORTMULTITHTTP_09_001: [ If the IoTHubMessage being sent contains property `content-type` it shall be added to the HTTP headers as "iothub-contenttype":"value". ]
    else if ((content_type = IoTHubMessage_GetContentTypeSystemProperty(message->messageHandle)) != NULL && HTTPHeaders_ReplaceHeaderNameValuePair(headers, IOTHUB_CONTENT_TYPE_D2C, content_type) != HTTP_HEADERS_OK)
    {
        LogError("unable to HTTPHeaders_ReplaceHeaderNameValuePair (content-type)");
        result = MU_FAILURE;
    }
    // Codes_SRS_TRANSPORTMULTITHTTP_09_002: [ If the IoTHubMessage being sent contains property `content-encoding` it shall be added to the HTTP headers as "iothub-contentencoding":"value". ]
    else if ((contentEncoding = IoTHubMessage_GetContentEncodingSystemProperty(message->messageHandle)) != NULL && HTTPHeaders_ReplaceHeaderNameValuePair(headers, IOTHUB_CONTENT_ENCODING_D2C, contentEncoding) != HTTP_HEADERS_OK)
    {
        LogError("unable to HTTPHeaders_ReplaceHeaderNameValuePair (content-encoding)");
        result = MU_FAILURE;
    }
    else if (IoTHubMessage_IsSecurityMessage(message->messageHandle) && HTTPHeaders_ReplaceHeaderNameValuePair(headers, SECURITY_INTERFACE_ID, SECURITY_INTERFACE_ID_VALUE) != HTTP_HEADERS_OK)
    {
        LogError("unable to set security message header info");
        result = MU_FAILURE;
    }
    else
    {
        result = 0;
    }
    return result;
}

static bool set_message_properties(IOTHUB_MESSAGE_LIST* message, size_t* msg_size, HTTP_HEADERS_HANDLE headers, HTTPTRANSPORT_HANDLE_DATA* handleData, HTTPTRANSPORT_PERDEVICE_DATA* deviceData)
{
    bool result = true;

    /*Codes_SRS_TRANSPORTMULTITHTTP_17_078: [Every message property "property":"value" shall be added to the HTTP headers as an individual header "iothub-app-property":"value".] */
    MAP_HANDLE map;
    const char*const* keys;
    const char*const* values;
    size_t count;
    if (message == NULL)
    {
        LogError("IOTHUB_MESSAGE_LIST is invalid");
        result = false;
    }
    else if ((map = IoTHubMessage_Properties(message->messageHandle)) == NULL)
    {
        LogError("MAP_HANDLE is invalid");
        result = false;
    }
    else if (Map_GetInternals(map, &keys, &values, &count) != MAP_OK)
    {
        /*Codes_SRS_TRANSPORTMULTITHTTP_17_079: [If any HTTP header operation fails, _DoWork shall advance to the next action.] */
        LogError("unable to Map_GetInternals");
        result = false;
    }
    else
    {
        for (size_t index = 0; index < count; index++)
        {
            /*Codes_SRS_TRANSPORTMULTITHTTP_17_074: [Every property name shall add  to the message size the length of the property name + the length of the property value + 16 bytes.] */
            *msg_size += (strlen(values[index]) + strlen(keys[index]) + MAXIMUM_PROPERTY_OVERHEAD);
            if (*msg_size > MAXIMUM_MESSAGE_SIZE)
            {
                /*Codes_SRS_TRANSPORTMULTITHTTP_17_072: [The message size shall be limited to 255KB -1 bytes.] */
                PDLIST_ENTRY head = DList_RemoveHeadList(deviceData->waitingToSend); /*actually this is the same as "actual", but now it is removed*/
                DList_InsertTailList(&(deviceData->eventConfirmations), head);
                handleData->transport_callbacks.send_complete_cb(&(deviceData->eventConfirmations), IOTHUB_CLIENT_CONFIRMATION_ERROR, deviceData->device_transport_ctx); // takes care of emptying the list too
                result = false;
                break;
            }
            else
            {
                STRING_HANDLE app_construct = STRING_construct_sprintf("%s%s", IOTHUB_APP_PREFIX, keys[index]);
                if (app_construct == NULL)
                {
                    /*Codes_SRS_TRANSPORTMULTITHTTP_17_079: [If any HTTP header operation fails, _DoWork shall advance to the next action.] */
                    LogError("Unable to construct app header");
                    result = false;
                    break;
                }
                else
                {
                    if (HTTPHeaders_ReplaceHeaderNameValuePair(headers, STRING_c_str(app_construct), values[index]) != HTTP_HEADERS_OK)
                    {
                        /*Codes_SRS_TRANSPORTMULTITHTTP_17_079: [If any HTTP header operation fails, _DoWork shall advance to the next action.] */
                        LogError("Unable to add app properties to http header");
                        result = false;
                        break;
                    }
                    STRING_delete(app_construct);
                }
            }
        }
    }
    return result;
}

static void destroy_messageHTTPrelativePath(HTTPTRANSPORT_PERDEVICE_DATA* handleData)
{
    STRING_delete(handleData->messageHTTPrelativePath);
    handleData->messageHTTPrelativePath = NULL;
}

static bool create_messageHTTPrelativePath(HTTPTRANSPORT_PERDEVICE_DATA* handleData, const char * deviceId)
{
    bool result;
    handleData->messageHTTPrelativePath = STRING_construct("/devices/");
    if (handleData->messageHTTPrelativePath == NULL)
    {
        LogError("STRING_construct failed.");
        result = false;
    }
    else
    {
        STRING_HANDLE urlEncodedDeviceId;
        /*Codes_SRS_TRANSPORTMULTITHTTP_17_019: [ IoTHubTransportHttp_Register shall create an immutable string (further called "message HTTP relative path") from the following pieces: "/devices/" + URL_ENCODED(deviceId) + "/messages/devicebound" + APIVERSION. ]*/
        if (!(
            ((urlEncodedDeviceId = URL_EncodeString(deviceId)) != NULL) &&
            (STRING_concat_with_STRING(handleData->messageHTTPrelativePath, urlEncodedDeviceId) == 0) &&
            (STRING_concat(handleData->messageHTTPrelativePath, MESSAGE_ENDPOINT_HTTP API_VERSION) == 0)
            ))
        {
            /*Codes_SRS_TRANSPORTMULTITHTTP_17_020: [ If creating the message HTTP relative path fails, then IoTHubTransportHttp_Register shall fail and return NULL. ]*/
            LogError("Creating HTTP message relative path failed.");
            result = false;
            destroy_messageHTTPrelativePath(handleData);
        }
        else
        {
            result = true;
        }
        STRING_delete(urlEncodedDeviceId);
    }

    return result;
}

static void destroy_eventHTTPrequestHeaders(HTTPTRANSPORT_PERDEVICE_DATA* handleData)
{
    HTTPHeaders_Free(handleData->eventHTTPrequestHeaders);
    handleData->eventHTTPrequestHeaders = NULL;
}

static HTTP_HEADERS_RESULT addUserAgentHeaderInfo(HTTPTRANSPORT_HANDLE_DATA* transport_data, HTTP_HEADERS_HANDLE eventHTTPrequestHeaders)
{
    HTTP_HEADERS_RESULT result;
    const char* product_info = transport_data->transport_callbacks.prod_info_cb(transport_data->transport_ctx);
    if (product_info == NULL)
    {
        result = HTTPHeaders_AddHeaderNameValuePair(eventHTTPrequestHeaders, "User-Agent", CLIENT_DEVICE_TYPE_PREFIX CLIENT_DEVICE_BACKSLASH IOTHUB_SDK_VERSION);
    }
    else
    {
        result = HTTPHeaders_AddHeaderNameValuePair(eventHTTPrequestHeaders, "User-Agent", product_info);
    }
    return result;
}

static bool create_eventHTTPrequestHeaders(HTTPTRANSPORT_PERDEVICE_DATA* handleData, HTTPTRANSPORT_HANDLE_DATA* transport_data, const char * deviceId, bool is_x509_used)
{
    /*Codes_SRS_TRANSPORTMULTITHTTP_17_021: [ IoTHubTransportHttp_Register shall create a set of HTTP headers (further called "event HTTP request headers") consisting of the following fixed field names and values: "iothub-to":"/devices/" + URL_ENCODED(deviceId) + "/messages/events"; "Authorization":""
    "Accept":"application/json"
    "Connection":"Keep-Alive" ]*/
    bool result;
    handleData->eventHTTPrequestHeaders = HTTPHeaders_Alloc();
    if (handleData->eventHTTPrequestHeaders == NULL)
    {
        LogError("HTTPHeaders_Alloc failed.");
        result = false;
    }
    else
    {
        STRING_HANDLE temp = STRING_construct("/devices/");
        if (temp == NULL)
        {
            LogError("STRING_construct failed.");
            result = false;
            destroy_eventHTTPrequestHeaders(handleData);
        }
        else
        {
            STRING_HANDLE urlEncodedDeviceId;
            if (!(
                ((urlEncodedDeviceId = URL_EncodeString(deviceId)) != NULL) &&
                (STRING_concat_with_STRING(temp, urlEncodedDeviceId) == 0) &&
                (STRING_concat(temp, EVENT_ENDPOINT) == 0)
                ))
            {
                LogError("deviceId construction failed.");
                result = false;
                destroy_eventHTTPrequestHeaders(handleData);
            }
            else
            {
                if (!(
                    (HTTPHeaders_AddHeaderNameValuePair(handleData->eventHTTPrequestHeaders, "iothub-to", STRING_c_str(temp)) == HTTP_HEADERS_OK) &&
                    (is_x509_used || (HTTPHeaders_AddHeaderNameValuePair(handleData->eventHTTPrequestHeaders, IOTHUB_AUTH_HEADER_VALUE, " ") == HTTP_HEADERS_OK)) &&
                    (HTTPHeaders_AddHeaderNameValuePair(handleData->eventHTTPrequestHeaders, "Accept", "application/json") == HTTP_HEADERS_OK) &&
                    (HTTPHeaders_AddHeaderNameValuePair(handleData->eventHTTPrequestHeaders, "Connection", "Keep-Alive") == HTTP_HEADERS_OK) &&
                    (addUserAgentHeaderInfo(transport_data, handleData->eventHTTPrequestHeaders) == HTTP_HEADERS_OK)
                    ))
                {
                    /*Codes_SRS_TRANSPORTMULTITHTTP_17_022: [ If creating the event HTTP request headers fails, then IoTHubTransportHttp_Register shall fail and return NULL.] */
                    LogError("adding header properties failed.");
                    result = false;
                    destroy_eventHTTPrequestHeaders(handleData);
                }
                else
                {
                    result = true;
                }
            }
            STRING_delete(urlEncodedDeviceId);
            STRING_delete(temp);
        }
    }
    return result;
}

static void destroy_messageHTTPrequestHeaders(HTTPTRANSPORT_PERDEVICE_DATA* handleData)
{
    HTTPHeaders_Free(handleData->messageHTTPrequestHeaders);
    handleData->messageHTTPrequestHeaders = NULL;
}

static bool create_messageHTTPrequestHeaders(HTTPTRANSPORT_PERDEVICE_DATA* handleData, HTTPTRANSPORT_HANDLE_DATA* transport_data, bool is_x509_used)
{
    /*Codes_SRS_TRANSPORTMULTITHTTP_17_132: [ IoTHubTransportHttp_Register shall create a set of HTTP headers (further called "message HTTP request headers") consisting of the following fixed field names and values:
    "Authorization": "" ]*/
    bool result;
    handleData->messageHTTPrequestHeaders = HTTPHeaders_Alloc();
    if (handleData->messageHTTPrequestHeaders == NULL)
    {
        LogError("HTTPHeaders_Alloc failed.");
        result = false;
    }
    else
    {
        if (!(
            (addUserAgentHeaderInfo(transport_data, handleData->messageHTTPrequestHeaders) == HTTP_HEADERS_OK) &&
            (is_x509_used || (HTTPHeaders_AddHeaderNameValuePair(handleData->messageHTTPrequestHeaders, IOTHUB_AUTH_HEADER_VALUE, " ") == HTTP_HEADERS_OK))
            ))
        {
            /*Codes_SRS_TRANSPORTMULTITHTTP_17_023: [ If creating message HTTP request headers then IoTHubTransportHttp_Register shall fail and return NULL. ]*/
            destroy_messageHTTPrequestHeaders(handleData);
            LogError("adding header properties failed.");
            result = false;
        }
        else
        {
            result = true;
        }
    }
    return result;
}

static void destroy_abandonHTTPrelativePathBegin(HTTPTRANSPORT_PERDEVICE_DATA* handleData)
{
    STRING_delete(handleData->abandonHTTPrelativePathBegin);
    handleData->abandonHTTPrelativePathBegin = NULL;
}

static bool create_abandonHTTPrelativePathBegin(HTTPTRANSPORT_PERDEVICE_DATA* handleData, const char * deviceId)
{
    /*Codes_SRS_TRANSPORTMULTITHTTP_17_024: [ IoTHubTransportHttp_Register shall create a STRING containing: "/devices/" + URL_ENCODED(device id) +"/messages/deviceBound/" called abandonHTTPrelativePathBegin. ]*/
    bool result;
    handleData->abandonHTTPrelativePathBegin = STRING_construct("/devices/");
    if (handleData->abandonHTTPrelativePathBegin == NULL)
    {
        result = false;
    }
    else
    {
        STRING_HANDLE urlEncodedDeviceId;
        if (!(
            ((urlEncodedDeviceId = URL_EncodeString(deviceId)) != NULL) &&
            (STRING_concat_with_STRING(handleData->abandonHTTPrelativePathBegin, urlEncodedDeviceId) == 0) &&
            (STRING_concat(handleData->abandonHTTPrelativePathBegin, MESSAGE_ENDPOINT_HTTP_ETAG) == 0)
            ))
        {
            /*Codes_SRS_TRANSPORTMULTITHTTP_17_025: [ If creating the abandonHTTPrelativePathBegin fails then IoTHubTransportHttp_Register shall fail and return NULL. ]*/
            LogError("unable to create abandon path string.");
            STRING_delete(handleData->abandonHTTPrelativePathBegin);
            result = false;
        }
        else
        {
            result = true;
        }
        STRING_delete(urlEncodedDeviceId);
    }
    return result;
}

static void destroy_SASObject(HTTPTRANSPORT_PERDEVICE_DATA* handleData)
{
    HTTPAPIEX_SAS_Destroy(handleData->sasObject);
    handleData->sasObject = NULL;
}

static bool create_deviceSASObject(HTTPTRANSPORT_PERDEVICE_DATA* handleData, STRING_HANDLE hostName, const char * deviceId, const char * deviceKey)
{
    STRING_HANDLE keyName;
    bool result;
    /*Codes_SRS_TRANSPORTMULTITHTTP_17_026: [IoTHubTransportHttp_Create shall invoke URL_EncodeString with an argument of device id.]*/
    keyName = URL_EncodeString(deviceId);
    if (keyName == NULL)
    {
        /*Codes_SRS_TRANSPORTMULTITHTTP_17_027: [If the encode fails then IoTHubTransportHttp_Create shall fail and return NULL.]*/
        LogError("URL_EncodeString keyname failed");
        result = false;
    }
    else
    {
        STRING_HANDLE uriResource;
        /*Codes_SRS_TRANSPORTMULTITHTTP_17_028: [IoTHubTransportHttp_Create shall invoke STRING_clone using the previously created hostname.]*/
        uriResource = STRING_clone(hostName);
        /*Codes_SRS_TRANSPORTMULTITHTTP_17_029: [If the clone fails then IoTHubTransportHttp_Create shall fail and return NULL.]*/
        if (uriResource != NULL)
        {
            /*Codes_SRS_TRANSPORTMULTITHTTP_17_030: [IoTHubTransportHttp_Create shall invoke STRING_concat with arguments uriResource and the string "/devices/".]*/
            /*Codes_SRS_TRANSPORTMULTITHTTP_17_141: [If the concat fails then IoTHubTransportHttp_Create shall fail and return NULL.]*/
            /*Codes_SRS_TRANSPORTMULTITHTTP_17_031: [IoTHubTransportHttp_Create shall invoke STRING_concat_with_STRING with arguments uriResource and keyName.]*/
            /*Codes_SRS_TRANSPORTMULTITHTTP_17_032: [If the STRING_concat_with_STRING fails then IoTHubTransportHttp_Create shall fail and return NULL.]*/
            if ((STRING_concat(uriResource, "/devices/") == 0) &&
                (STRING_concat_with_STRING(uriResource, keyName) == 0))
            {
                /*Codes_SRS_TRANSPORTMULTITHTTP_17_033: [IoTHubTransportHttp_Create shall invoke STRING_construct with an argument of config->upperConfig->deviceKey.]*/
                /*Codes_SRS_TRANSPORTMULTITHTTP_17_034: [If the STRING_construct fails then IoTHubTransportHttp_Create shall fail and return NULL.]*/
                STRING_HANDLE key = STRING_construct(deviceKey);
                if (key != NULL)
                {
                    /*Codes_SRS_TRANSPORTMULTITHTTP_17_035: [The keyName is shortened to zero length, if that fails then IoTHubTransportHttp_Create shall fail and return NULL.]*/
                    if (STRING_empty(keyName) != 0)
                    {
                        LogError("Unable to form the device key name for the SAS");
                        result = false;
                    }
                    else
                    {
                        /*Codes_SRS_TRANSPORTMULTITHTTP_17_036: [IoTHubTransportHttp_Create shall invoke HTTPAPIEX_SAS_Create with arguments key, uriResource, and zero length keyName.]*/
                        /*Codes_SRS_TRANSPORTMULTITHTTP_17_037: [If the HTTPAPIEX_SAS_Create fails then IoTHubTransportHttp_Create shall fail and return NULL.]*/
                        handleData->sasObject = HTTPAPIEX_SAS_Create(key, uriResource, keyName);
                        result = (handleData->sasObject != NULL) ? (true) : (false);
                    }
                    STRING_delete(key);
                }
                else
                {
                    LogError("STRING_construct Key failed");
                    result = false;
                }
            }
            else
            {
                LogError("STRING_concat uri resource failed");
                result = false;
            }
            STRING_delete(uriResource);
        }
        else
        {
            LogError("STRING_staticclone uri resource failed");
            result = false;
        }
        STRING_delete(keyName);
    }
    return result;
}

static void destroy_deviceId(HTTPTRANSPORT_PERDEVICE_DATA* handleData)
{
    STRING_delete(handleData->deviceId);
    handleData->deviceId = NULL;
}

static bool create_deviceId(HTTPTRANSPORT_PERDEVICE_DATA* handleData, const char * deviceId)
{
    /*Codes_SRS_TRANSPORTMULTITHTTP_17_133: [ IoTHubTransportHttp_Register shall create an immutable string (further called "deviceId") from config->deviceConfig->deviceId. ]*/
    bool result;
    handleData->deviceId = STRING_construct(deviceId);
    if (handleData->deviceId == NULL)
    {
        /*Codes_SRS_TRANSPORTMULTITHTTP_17_134: [ If deviceId is not created, then IoTHubTransportHttp_Register shall fail and return NULL. */
        LogError("STRING_construct deviceId failed");
        result = false;
    }
    else
    {
        result = true;
    }
    return result;
}

static void destroy_deviceKey(HTTPTRANSPORT_PERDEVICE_DATA* handleData)
{
    STRING_delete(handleData->deviceKey);
    handleData->deviceKey = NULL;
}

static void destroy_deviceSas(HTTPTRANSPORT_PERDEVICE_DATA* handleData)
{
    STRING_delete(handleData->deviceSasToken);
    handleData->deviceSasToken = NULL;
}

static bool create_deviceKey(HTTPTRANSPORT_PERDEVICE_DATA* handleData, const char * deviceKey)
{
    /*Codes_SRS_TRANSPORTMULTITHTTP_17_135: [ IoTHubTransportHttp_Register shall create an immutable string (further called "deviceKey") from deviceKey. ]*/
    bool result;
    handleData->deviceKey = STRING_construct(deviceKey);
    if (handleData->deviceKey == NULL)
    {
        /*Codes_SRS_TRANSPORTMULTITHTTP_17_136: [ If deviceKey is not created, then IoTHubTransportHttp_Register shall fail and return NULL. ]*/
        LogError("STRING_construct deviceKey failed");
        result = false;
    }
    else
    {
        result = true;
    }
    return result;
}

static void destroy_deviceSasToken(HTTPTRANSPORT_PERDEVICE_DATA* handleData)
{
    STRING_delete(handleData->deviceSasToken);
    handleData->deviceSasToken = NULL;
}

static bool create_deviceSasToken(HTTPTRANSPORT_PERDEVICE_DATA* handleData, const char * deviceSasToken)
{
    /*Codes_SRS_TRANSPORTMULTITHTTP_03_135: [ IoTHubTransportHttp_Register shall create an immutable string (further called "deviceSasToken") from deviceSasToken. ]*/
    bool result;
    handleData->deviceSasToken = STRING_construct(deviceSasToken);
    if (handleData->deviceSasToken == NULL)
    {
        /*Codes_SRS_TRANSPORTMULTITHTTP_03_136: [ If deviceSasToken is not created, then IoTHubTransportHttp_Register shall fail and return NULL. ]*/
        LogError("STRING_construct deviceSasToken failed");
        result = false;
    }
    else
    {
        result = true;
    }
    return result;
}

/*
* List queries  Find by handle and find by device name
*/

/*Codes_SRS_TRANSPORTMULTITHTTP_17_137: [ IoTHubTransportHttp_Register shall search the devices list for any device matching name deviceId. If deviceId is found it shall return NULL. ]*/
static bool findDeviceHandle(const void* element, const void* value)
{
    bool result;
    /* data stored at element is device handle */
    const IOTHUB_DEVICE_HANDLE * guess = (const IOTHUB_DEVICE_HANDLE *)element;
    IOTHUB_DEVICE_HANDLE match = (IOTHUB_DEVICE_HANDLE)value;
    result = (*guess == match) ? true : false;
    return result;
}

static bool findDeviceById(const void* element, const void* value)
{
    bool result;
    const char* deviceId = (const char *)value;
    const HTTPTRANSPORT_PERDEVICE_DATA * perDeviceElement = *(const HTTPTRANSPORT_PERDEVICE_DATA **)element;

    result = (strcmp(STRING_c_str(perDeviceElement->deviceId), deviceId) == 0);

    return result;
}

static IOTHUB_DEVICE_HANDLE IoTHubTransportHttp_Register(TRANSPORT_LL_HANDLE handle, const IOTHUB_DEVICE_CONFIG* device, PDLIST_ENTRY waitingToSend)
{
    HTTPTRANSPORT_PERDEVICE_DATA* result;
    if (handle == NULL || device == NULL)
    {
        /*Codes_SRS_TRANSPORTMULTITHTTP_17_142: [ If handle is NULL or device is NULL, then IoTHubTransportHttp_Register shall return NULL. ]*/
        LogError("Transport handle is NULL");
        result = NULL;
    }
    else if (device->deviceId == NULL || ((device->deviceKey != NULL) && (device->deviceSasToken != NULL)) || waitingToSend == NULL)
    {
        /*Codes_SRS_TRANSPORTMULTITHTTP_17_015: [ If IOTHUB_DEVICE_CONFIG fields deviceKey and deviceSasToken are NULL, then IoTHubTransportHttp_Register shall assume a x509 authentication. ]*/
        /*Codes_SRS_TRANSPORTMULTITHTTP_17_014: [ If IOTHUB_DEVICE_CONFIG field deviceId is NULL, then IoTHubTransportHttp_Register shall return NULL. ]*/
        /*Codes_SRS_TRANSPORTMULTITHTTP_17_016: [ If parameter waitingToSend is NULL, then IoTHubTransportHttp_Register shall return NULL. ]*/
        LogError("invalid parameters detected TRANSPORT_LL_HANDLE handle=%p, const IOTHUB_DEVICE_CONFIG* device=%p, PDLIST_ENTRY waitingToSend=%p",
            handle, device, waitingToSend);
        result = NULL;
    }
    else
    {
        HTTPTRANSPORT_HANDLE_DATA* handleData = (HTTPTRANSPORT_HANDLE_DATA*)handle;
        /*Codes_SRS_TRANSPORTMULTITHTTP_17_137: [ IoTHubTransportHttp_Register shall search the devices list for any device matching name deviceId. If deviceId is found it shall return NULL. ]*/
        void* listItem = VECTOR_find_if(handleData->perDeviceList, findDeviceById, device->deviceId);
        if (listItem != NULL)
        {
            /*Codes_SRS_TRANSPORTMULTITHTTP_17_137: [ IoTHubTransportHttp_Register shall search the devices list for any device matching name deviceId. If deviceId is found it shall return NULL. ]*/
            LogError("Transport already has device registered by id: [%s]", device->deviceId);
            result = NULL;
        }
        else
        {
            bool was_create_deviceKey_ok = false;
            bool was_create_deviceSasToken_ok = false;
            bool was_sasObject_ok = false;
            bool was_x509_ok = false; /*there's nothing "created" in the case of x509, it is a flag indicating that x509 is used*/

            /*Codes_SRS_TRANSPORTMULTITHTTP_17_038: [ Otherwise, IoTHubTransportHttp_Register shall allocate the IOTHUB_DEVICE_HANDLE structure. ]*/
            bool was_resultCreated_ok = ((result = (HTTPTRANSPORT_PERDEVICE_DATA *)malloc(sizeof(HTTPTRANSPORT_PERDEVICE_DATA))) != NULL);
            bool was_create_deviceId_ok = was_resultCreated_ok && create_deviceId(result, device->deviceId);

            if (was_create_deviceId_ok)
            {
                if (device->deviceSasToken != NULL)
                {
                    /*device->deviceKey is certainly NULL (because of the validation condition (else if (device->deviceId == NULL || ((device->deviceKey != NULL) && (device->deviceSasToken != NULL)) || waitingToSend == NULL))that does not accept that both satoken AND devicekey are non-NULL*/
                    was_create_deviceSasToken_ok = create_deviceSasToken(result, device->deviceSasToken);
                    result->deviceKey = NULL;
                    result->sasObject = NULL;
                }
                else /*when deviceSasToken == NULL*/
                {
                    if (device->deviceKey != NULL)
                    {
                        was_create_deviceKey_ok = create_deviceKey(result, device->deviceKey);
                        result->deviceSasToken = NULL;
                    }
                    else
                    {
                        /*when both of them are NULL*/
                        was_x509_ok = true;
                        result->deviceKey = NULL;
                        result->deviceSasToken = NULL;
                    }
                }
            }

            bool was_eventHTTPrelativePath_ok = (was_create_deviceKey_ok || was_create_deviceSasToken_ok || was_x509_ok) && create_eventHTTPrelativePath(result, device->deviceId);
            bool was_messageHTTPrelativePath_ok = was_eventHTTPrelativePath_ok && create_messageHTTPrelativePath(result, device->deviceId);
            bool was_eventHTTPrequestHeaders_ok;
            if (was_messageHTTPrelativePath_ok)
            {
                result->device_transport_ctx = handleData->transport_ctx;
                was_eventHTTPrequestHeaders_ok = create_eventHTTPrequestHeaders(result, handleData, device->deviceId, was_x509_ok);
            }
            else
            {
                was_eventHTTPrequestHeaders_ok = false;
            }
            bool was_messageHTTPrequestHeaders_ok = was_eventHTTPrequestHeaders_ok && create_messageHTTPrequestHeaders(result, handleData, was_x509_ok);
            bool was_abandonHTTPrelativePathBegin_ok = was_messageHTTPrequestHeaders_ok && create_abandonHTTPrelativePathBegin(result, device->deviceId);

            if (was_x509_ok)
            {
                result->sasObject = NULL; /**/
                was_sasObject_ok = true;
            }
            else
            {
                if (!was_create_deviceSasToken_ok)
                {
                    was_sasObject_ok = was_abandonHTTPrelativePathBegin_ok && create_deviceSASObject(result, handleData->hostName, device->deviceId, device->deviceKey);
                }
            }

            /*Codes_SRS_TRANSPORTMULTITHTTP_17_041: [ IoTHubTransportHttp_Register shall call VECTOR_push_back to store the new device information. ]*/
            bool was_list_add_ok = (was_sasObject_ok || was_create_deviceSasToken_ok || was_x509_ok) && (VECTOR_push_back(handleData->perDeviceList, &result, 1) == 0);

            if (was_list_add_ok)
            {
                /*Codes_SRS_TRANSPORTMULTITHTTP_17_043: [ Upon success, IoTHubTransportHttp_Register shall store the transport handle, and the waitingToSend queue in the device handle return a non-NULL value. ]*/
                /*Codes_SRS_TRANSPORTMULTITHTTP_17_040: [ IoTHubTransportHttp_Register shall put event HTTP relative path, message HTTP relative path, event HTTP request headers, message HTTP request headers, abandonHTTPrelativePathBegin, HTTPAPIEX_SAS_HANDLE, and the device handle into a device structure. ]*/
                /*Codes_SRS_TRANSPORTMULTITHTTP_17_128: [ IoTHubTransportHttp_Register shall mark this device as unsubscribed. ]*/
                result->DoWork_PullMessage = false;
                result->isFirstPoll = true;
                result->waitingToSend = waitingToSend;
                DList_InitializeListHead(&(result->eventConfirmations));
                result->transportHandle = (HTTPTRANSPORT_HANDLE_DATA *)handle;
            }
            else
            {
                /*Codes_SRS_TRANSPORTMULTITHTTP_17_042: [ If the singlylinkedlist_add fails then IoTHubTransportHttp_Register shall fail and return NULL. ]*/
                if (was_sasObject_ok) destroy_SASObject(result);
                if (was_abandonHTTPrelativePathBegin_ok) destroy_abandonHTTPrelativePathBegin(result);
                if (was_messageHTTPrelativePath_ok) destroy_messageHTTPrelativePath(result);
                if (was_eventHTTPrequestHeaders_ok) destroy_eventHTTPrequestHeaders(result);
                if (was_messageHTTPrequestHeaders_ok) destroy_messageHTTPrequestHeaders(result);
                if (was_eventHTTPrelativePath_ok) destroy_eventHTTPrelativePath(result);
                if (was_create_deviceId_ok) destroy_deviceId(result);
                if (was_create_deviceKey_ok) destroy_deviceKey(result);
                if (was_create_deviceSasToken_ok) destroy_deviceSasToken(result);
                /*Codes_SRS_TRANSPORTMULTITHTTP_17_039: [ If the allocating the device handle fails then IoTHubTransportHttp_Register shall fail and return NULL. ] */
                if (was_resultCreated_ok) free(result);
                result = NULL;
            }
        }

    }
    return (IOTHUB_DEVICE_HANDLE)result;
}


static void destroy_perDeviceData(HTTPTRANSPORT_PERDEVICE_DATA * perDeviceItem)
{
    destroy_deviceId(perDeviceItem);
    destroy_deviceKey(perDeviceItem);
    destroy_deviceSas(perDeviceItem);
    destroy_eventHTTPrelativePath(perDeviceItem);
    destroy_messageHTTPrelativePath(perDeviceItem);
    destroy_eventHTTPrequestHeaders(perDeviceItem);
    destroy_messageHTTPrequestHeaders(perDeviceItem);
    destroy_abandonHTTPrelativePathBegin(perDeviceItem);
    destroy_SASObject(perDeviceItem);
}

static IOTHUB_DEVICE_HANDLE* get_perDeviceDataItem(IOTHUB_DEVICE_HANDLE deviceHandle)
{
    HTTPTRANSPORT_PERDEVICE_DATA* deviceHandleData = (HTTPTRANSPORT_PERDEVICE_DATA*)deviceHandle;
    IOTHUB_DEVICE_HANDLE* listItem;

    HTTPTRANSPORT_HANDLE_DATA* handleData = deviceHandleData->transportHandle;

    listItem = (IOTHUB_DEVICE_HANDLE *)VECTOR_find_if(handleData->perDeviceList, findDeviceHandle, deviceHandle);
    if (listItem == NULL)
    {
        LogError("device handle not found in transport device list");
        listItem = NULL;
    }
    else
    {
        /* sucessfully found device in list. */
    }

    return listItem;
}

static void IoTHubTransportHttp_Unregister(IOTHUB_DEVICE_HANDLE deviceHandle)
{
    if (deviceHandle == NULL)
    {
        /*Codes_SRS_TRANSPORTMULTITHTTP_17_044: [ If deviceHandle is NULL, then IoTHubTransportHttp_Unregister shall do nothing. ]*/
        LogError("Unregister a NULL device handle");
    }
    else
    {
        HTTPTRANSPORT_PERDEVICE_DATA* deviceHandleData = (HTTPTRANSPORT_PERDEVICE_DATA*)deviceHandle;
        HTTPTRANSPORT_HANDLE_DATA* handleData = deviceHandleData->transportHandle;
        /*Codes_SRS_TRANSPORTMULTITHTTP_17_045: [ IoTHubTransportHttp_Unregister shall locate deviceHandle in the transport device list by calling list_find_if. ]*/
        IOTHUB_DEVICE_HANDLE* listItem = get_perDeviceDataItem(deviceHandle);
        if (listItem == NULL)
        {
            /*Codes_SRS_TRANSPORTMULTITHTTP_17_046: [ If the device structure is not found, then this function shall fail and do nothing. ]*/
            LogError("Device Handle [%p] not found in transport", deviceHandle);
        }
        else
        {
            HTTPTRANSPORT_PERDEVICE_DATA * perDeviceItem = (HTTPTRANSPORT_PERDEVICE_DATA *)(*listItem);

            /*Codes_SRS_TRANSPORTMULTITHTTP_17_047: [ IoTHubTransportHttp_Unregister shall free all the resources used in the device structure. ]*/
            destroy_perDeviceData(perDeviceItem);
            /*Codes_SRS_TRANSPORTMULTITHTTP_17_048: [ IoTHubTransportHttp_Unregister shall call singlylinkedlist_remove to remove device from devices list. ]*/
            VECTOR_erase(handleData->perDeviceList, listItem, 1);
            free(deviceHandleData);
        }
    }

    return;
}

/*Codes_SRS_TRANSPORTMULTITHTTP_17_005: [If config->upperConfig->protocolGatewayHostName is NULL, `IoTHubTransportHttp_Create` shall create an immutable string (further called hostname) containing `config->transportConfig->iotHubName + config->transportConfig->iotHubSuffix`.]  */
/*Codes_SRS_TRANSPORTMULTITHTTP_20_001: [If config->upperConfig->protocolGatewayHostName is not NULL, IoTHubTransportHttp_Create shall use it as hostname] */
static void destroy_hostName(HTTPTRANSPORT_HANDLE_DATA* handleData)
{
    STRING_delete(handleData->hostName);
    handleData->hostName = NULL;
}

static bool create_hostName(HTTPTRANSPORT_HANDLE_DATA* handleData, const IOTHUBTRANSPORT_CONFIG* config)
{
    bool result;
    if (config->upperConfig->protocolGatewayHostName != NULL)
    {
        /*Codes_SRS_TRANSPORTMULTITHTTP_20_001: [If config->upperConfig->protocolGatewayHostName is not NULL, IoTHubTransportHttp_Create shall use it as hostname] */
        handleData->hostName = STRING_construct(config->upperConfig->protocolGatewayHostName);
        result = (handleData->hostName != NULL);
    }
    else
    {
        /*Codes_SRS_TRANSPORTMULTITHTTP_17_005: [If config->upperConfig->protocolGatewayHostName is NULL, `IoTHubTransportHttp_Create` shall create an immutable string (further called hostname) containing `config->transportConfig->iotHubName + config->transportConfig->iotHubSuffix`.]  */
        handleData->hostName = STRING_construct(config->upperConfig->iotHubName);

        if (handleData->hostName == NULL)
        {
            result = false;
        }
        else
        {
            if ((STRING_concat(handleData->hostName, ".") != 0) ||
                (STRING_concat(handleData->hostName, config->upperConfig->iotHubSuffix) != 0))
            {
                /*Codes_SRS_TRANSPORTMULTITHTTP_17_006: [ If creating the hostname fails then IoTHubTransportHttp_Create shall fail and return NULL. ] */
                destroy_hostName(handleData);
                result = false;
            }
            else
            {
                result = true;
            }
        }
    }
    return result;
}

/*Codes_SRS_TRANSPORTMULTITHTTP_17_007: [Otherwise, IoTHubTransportHttp_Create shall create a HTTPAPIEX_HANDLE by a call to HTTPAPIEX_Create passing for hostName the hostname so far constructed by IoTHubTransportHttp_Create.]*/
static void destroy_httpApiExHandle(HTTPTRANSPORT_HANDLE_DATA* handleData)
{
    HTTPAPIEX_Destroy(handleData->httpApiExHandle);
    handleData->httpApiExHandle = NULL;
}

/*Codes_SRS_TRANSPORTMULTITHTTP_17_007: [ IoTHubTransportHttp_Create shall create a HTTPAPIEX_HANDLE by a call to HTTPAPIEX_Create passing for hostName the hostname so far constructed by IoTHubTransportHttp_Create. ]*/
static bool create_httpApiExHandle(HTTPTRANSPORT_HANDLE_DATA* handleData, const IOTHUBTRANSPORT_CONFIG* config)
{
    bool result;
    (void)config;
    handleData->httpApiExHandle = HTTPAPIEX_Create(STRING_c_str(handleData->hostName));
    if (handleData->httpApiExHandle == NULL)
    {
        /*Codes_SRS_TRANSPORTMULTITHTTP_17_008: [ If creating the HTTPAPIEX_HANDLE fails then IoTHubTransportHttp_Create shall fail and return NULL. ] */
        result = false;
    }
    else
    {
        result = true;
    }
    return result;
}

static void destroy_perDeviceList(HTTPTRANSPORT_HANDLE_DATA* handleData)
{
    VECTOR_destroy(handleData->perDeviceList);
    handleData->perDeviceList = NULL;
}

/*Codes_SRS_TRANSPORTMULTITHTTP_17_009: [ IoTHubTransportHttp_Create shall call singlylinkedlist_create to create a list of registered devices. ]*/
static bool create_perDeviceList(HTTPTRANSPORT_HANDLE_DATA* handleData)
{
    bool result;
    handleData->perDeviceList = VECTOR_create(sizeof(IOTHUB_DEVICE_HANDLE));
    if (handleData == NULL || handleData->perDeviceList == NULL)
    {
        /*Codes_SRS_TRANSPORTMULTITHTTP_17_010: [ If creating the list fails, then IoTHubTransportHttp_Create shall fail and return NULL. ]*/
        result = false;
    }
    else
    {
        result = true;
    }
    return result;
}

static TRANSPORT_LL_HANDLE IoTHubTransportHttp_Create(const IOTHUBTRANSPORT_CONFIG* config, TRANSPORT_CALLBACKS_INFO* cb_info, void* ctx)
{
    HTTPTRANSPORT_HANDLE_DATA* result;
    if (config == NULL)
    {
        /*Codes_SRS_TRANSPORTMULTITHTTP_17_001: [If parameter config is NULL, then IoTHubTransportHttp_Create shall return NULL.]*/
        LogError("invalid arg (configuration is missing)");
        result = NULL;
    }
    else if (config->upperConfig == NULL)
    {
        /*Codes_SRS_TRANSPORTMULTITHTTP_17_002: [ If field transportConfig is NULL, then IoTHubTransportHttp_Create shall return NULL. ]*/
        LogError("invalid arg (upperConfig is NULL)");
        result = NULL;
    }
    else if (config->upperConfig->protocol == NULL)
    {
        /*Codes_SRS_TRANSPORTMULTITHTTP_17_003: [ If fields protocol, iotHubName or iotHubSuffix in transportConfig are NULL, then IoTHubTransportHttp_Create shall return NULL. ]*/
        LogError("invalid arg (protocol is NULL)");
        result = NULL;
    }
    else if (config->upperConfig->iotHubName == NULL)
    {
        /*Codes_SRS_TRANSPORTMULTITHTTP_17_003: [ If fields protocol, iotHubName or iotHubSuffix in transportConfig are NULL, then IoTHubTransportHttp_Create shall return NULL. ]*/
        LogError("invalid arg (iotHubName is NULL)");
        result = NULL;
    }
    else if (config->upperConfig->iotHubSuffix == NULL)
    {
        /*Codes_SRS_TRANSPORTMULTITHTTP_17_003: [ If fields protocol, iotHubName or iotHubSuffix in transportConfig are NULL, then IoTHubTransportHttp_Create shall return NULL. ]*/
        LogError("invalid arg (iotHubSuffix is NULL)");
        result = NULL;
    }
    else if (IoTHub_Transport_ValidateCallbacks(cb_info) != 0)
    {
        LogError("Invalid transport callback information");
        result = NULL;
    }
    else
    {
        /*Codes_SRS_TRANSPORTMULTITHTTP_17_130: [ IoTHubTransportHttp_Create shall allocate memory for the handle. ]*/
        result = (HTTPTRANSPORT_HANDLE_DATA*)malloc(sizeof(HTTPTRANSPORT_HANDLE_DATA));
        if (result == NULL)
        {
            /*Codes_SRS_TRANSPORTMULTITHTTP_17_131: [ If allocation fails, IoTHubTransportHttp_Create shall fail and return NULL. ]*/
            LogError("unable to malloc");
        }
        else if (HTTPAPIEX_Init() == HTTPAPIEX_ERROR)
        {
            /*Codes_SRS_TRANSPORTMULTITHTTP_21_143: [ If HTTPAPIEX_Init fails, IoTHubTransportHttp_Create shall fail and return NULL. ]*/
            LogError("Error initializing HTTP");
            free(result);
            result = NULL;
        }
        else
        {
            bool was_hostName_ok = create_hostName(result, config);
            bool was_httpApiExHandle_ok = was_hostName_ok && create_httpApiExHandle(result, config);
            bool was_perDeviceList_ok = was_httpApiExHandle_ok && create_perDeviceList(result);

            if (was_perDeviceList_ok)
            {
                /*Codes_SRS_TRANSPORTMULTITHTTP_17_011: [ Otherwise, IoTHubTransportHttp_Create shall succeed and return a non-NULL value. ]*/
                result->doBatchedTransfers = false;
                result->getMinimumPollingTime = DEFAULT_GETMINIMUMPOLLINGTIME;

                result->transport_ctx = ctx;
                memcpy(&result->transport_callbacks, cb_info, sizeof(TRANSPORT_CALLBACKS_INFO));
            }
            else
            {
                if (was_httpApiExHandle_ok) destroy_httpApiExHandle(result);
                if (was_hostName_ok) destroy_hostName(result);
                HTTPAPIEX_Deinit();

                free(result);
                result = NULL;
            }
        }
    }
    return result;
}

static void IoTHubTransportHttp_Destroy(TRANSPORT_LL_HANDLE handle)
{
    /*Codes_SRS_TRANSPORTMULTITHTTP_17_012: [ IoTHubTransportHttp_Destroy shall do nothing is handle is NULL. ]*/
    if (handle != NULL)
    {
        HTTPTRANSPORT_HANDLE_DATA* handleData = (HTTPTRANSPORT_HANDLE_DATA*)handle;
        IOTHUB_DEVICE_HANDLE* listItem;

        size_t deviceListSize = VECTOR_size(handleData->perDeviceList);

        /*Codes_SRS_TRANSPORTMULTITHTTP_17_013: [ Otherwise, IoTHubTransportHttp_Destroy shall free all the resources currently in use. ]*/
        for (size_t i = 0; i < deviceListSize; i++)
        {
            listItem = (IOTHUB_DEVICE_HANDLE *)VECTOR_element(handleData->perDeviceList, i);
            HTTPTRANSPORT_PERDEVICE_DATA* perDeviceItem = (HTTPTRANSPORT_PERDEVICE_DATA*)(*listItem);
            destroy_perDeviceData(perDeviceItem);
            free(perDeviceItem);
        }

        destroy_hostName((HTTPTRANSPORT_HANDLE_DATA *)handle);
        destroy_httpApiExHandle((HTTPTRANSPORT_HANDLE_DATA *)handle);
        destroy_perDeviceList((HTTPTRANSPORT_HANDLE_DATA *)handle);
        HTTPAPIEX_Deinit();
        free(handle);
    }
}

static int IoTHubTransportHttp_Subscribe(IOTHUB_DEVICE_HANDLE handle)
{
    int result;
    if (handle == NULL)
    {
        /*Codes_SRS_TRANSPORTMULTITHTTP_17_103: [ If parameter deviceHandle is NULL then IoTHubTransportHttp_Subscribe shall fail and return a non-zero value. ]*/
        LogError("invalid arg passed to IoTHubTransportHttp_Subscribe");
        result = MU_FAILURE;
    }
    else
    {
        /*Codes_SRS_TRANSPORTMULTITHTTP_17_104: [ IoTHubTransportHttp_Subscribe shall locate deviceHandle in the transport device list by calling list_find_if. ]*/
        IOTHUB_DEVICE_HANDLE* listItem = get_perDeviceDataItem(handle);

        if (listItem == NULL)
        {
            /*Codes_SRS_TRANSPORTMULTITHTTP_17_105: [ If the device structure is not found, then this function shall fail and return a non-zero value. ]*/
            LogError("did not find device in transport handle");
            result = MU_FAILURE;
        }
        else
        {
            HTTPTRANSPORT_PERDEVICE_DATA * perDeviceItem;

            perDeviceItem = (HTTPTRANSPORT_PERDEVICE_DATA *)(*listItem);
            /*Codes_SRS_TRANSPORTMULTITHTTP_17_106: [ Otherwise, IoTHubTransportHttp_Subscribe shall set the device so that subsequent calls to DoWork should execute HTTP requests. ]*/
            perDeviceItem->DoWork_PullMessage = true;
        }
        result = 0;
    }
    return result;
}

static void IoTHubTransportHttp_Unsubscribe(IOTHUB_DEVICE_HANDLE handle)
{
    /*Codes_SRS_TRANSPORTMULTITHTTP_17_107: [ If parameter deviceHandle is NULL then IoTHubTransportHttp_Unsubscribe shall fail do nothing. ]*/
    if (handle != NULL)
    {
        /*Codes_SRS_TRANSPORTMULTITHTTP_17_108: [ IoTHubTransportHttp_Unsubscribe shall locate deviceHandle in the transport device list by calling list_find_if. ]*/
        IOTHUB_DEVICE_HANDLE* listItem = get_perDeviceDataItem(handle);
        /*Codes_SRS_TRANSPORTMULTITHTTP_17_109: [ If the device structure is not found, then this function shall fail and do nothing. ]*/
        if (listItem != NULL)
        {
            HTTPTRANSPORT_PERDEVICE_DATA * perDeviceItem = (HTTPTRANSPORT_PERDEVICE_DATA *)(*listItem);
            /*Codes_SRS_TRANSPORTMULTITHTTP_17_110: [ Otherwise, IoTHubTransportHttp_Subscribe shall set the device so that subsequent calls to DoWork shall not execute HTTP requests. ]*/
            perDeviceItem->DoWork_PullMessage = false;
        }
        else
        {
            LogError("Device not found to unsuscribe.");
        }
    }
    else
    {
        LogError("Null handle passed to Unsuscribe.");
    }
}

static int IoTHubTransportHttp_Subscribe_DeviceTwin(IOTHUB_DEVICE_HANDLE handle)
{
    /*Codes_SRS_TRANSPORTMULTITHTTP_02_003: [ IoTHubTransportHttp_Subscribe_DeviceTwin shall return a non-zero value. ]*/
    (void)handle;
    int result = MU_FAILURE;
    LogError("IoTHubTransportHttp_Subscribe_DeviceTwin Not supported");
    return result;
}

static void IoTHubTransportHttp_Unsubscribe_DeviceTwin(IOTHUB_DEVICE_HANDLE handle)
{
    (void)handle;
    /*Codes_SRS_TRANSPORTMULTITHTTP_02_004: [ IoTHubTransportHttp_Unsubscribe_DeviceTwin shall return ]*/
    LogError("IoTHubTransportHttp_Unsubscribe_DeviceTwin Not supported");
}

static IOTHUB_CLIENT_RESULT IoTHubTransportHttp_GetTwinAsync(IOTHUB_DEVICE_HANDLE handle, IOTHUB_CLIENT_DEVICE_TWIN_CALLBACK completionCallback, void* callbackContext)
{
    (void)handle;
    (void)completionCallback;
    (void)callbackContext;
    LogError("Currently Not Supported.");
    // Codes_SRS_TRANSPORTMULTITHTTP_09_005: [ `IoTHubTransportHttp_GetTwinAsync` shall return IOTHUB_CLIENT_ERROR]
    return IOTHUB_CLIENT_ERROR;
}

static int IoTHubTransportHttp_Subscribe_DeviceMethod(IOTHUB_DEVICE_HANDLE handle)
{
    (void)handle;
    int result = MU_FAILURE;
    LogError("not implemented (yet)");
    return result;
}

static void IoTHubTransportHttp_Unsubscribe_DeviceMethod(IOTHUB_DEVICE_HANDLE handle)
{
    (void)handle;
    LogError("not implemented (yet)");
}

static int IoTHubTransportHttp_DeviceMethod_Response(IOTHUB_DEVICE_HANDLE handle, METHOD_HANDLE methodId, const unsigned char* response, size_t response_size, int status_response)
{
    (void)handle;
    (void)methodId;
    (void)response;
    (void)response_size;
    (void)status_response;
    LogError("not implemented (yet)");
    return MU_FAILURE;
}

/*produces a representation of the properties, if they exist*/
/*if they do not exist, produces ""*/
static int concat_Properties(STRING_HANDLE existing, MAP_HANDLE map, size_t* propertiesMessageSizeContribution)
{
    int result;
    const char*const* keys;
    const char*const* values;
    size_t count;
    if (Map_GetInternals(map, &keys, &values, &count) != MAP_OK)
    {
        result = MU_FAILURE;
        LogError("error while Map_GetInternals");
    }
    else
    {

        if (count == 0)
        {
            /*Codes_SRS_TRANSPORTMULTITHTTP_17_064: [If IoTHubMessage does not have properties, then "properties":{...} shall be missing from the payload*/
            /*no properties - do nothing with existing*/
            result = 0;
            *propertiesMessageSizeContribution = 0;
        }
        else
        {
            /*Codes_SRS_TRANSPORTMULTITHTTP_17_058: [If IoTHubMessage has properties, then they shall be serialized at the same level as "body" using the following pattern: "properties":{"iothub-app-name1":"value1","iothub-app-name2":"value2*/
            if (STRING_concat(existing, ",\"properties\":") != 0)
            {
                /*go ahead and return it*/
                result = MU_FAILURE;
                LogError("failed STRING_concat");
            }
            else if (appendMapToJSON(existing, keys, values, count) != 0)
            {
                result = MU_FAILURE;
                LogError("unable to append the properties");
            }
            else
            {
                /*all is fine*/
                size_t i;
                *propertiesMessageSizeContribution = 0;
                for (i = 0; i < count; i++)
                {
                    /*Codes_SRS_TRANSPORTMULTITHTTP_17_063: [Every property name shall add to the message size the length of the property name + the length of the property value + 16 bytes.] */
                    *propertiesMessageSizeContribution += (strlen(keys[i]) + strlen(values[i]) + MAXIMUM_PROPERTY_OVERHEAD);
                }
                result = 0;
            }
        }
    }
    return result;
}

/*produces a JSON representation of the map : {"a": "value_of_a","b":"value_of_b"}*/
static int appendMapToJSON(STRING_HANDLE existing, const char* const* keys, const char* const* values, size_t count) /*under consideration: move to MAP module when it has more than 1 user*/
{
    int result;
    if (STRING_concat(existing, "{") != 0)
    {
        /*go on and return it*/
        LogError("STRING_construct failed");
        result = MU_FAILURE;
    }
    else
    {
        size_t i;
        for (i = 0; i < count; i++)
        {
            if (!(
                (STRING_concat(existing, (i == 0) ? "\"" IOTHUB_APP_PREFIX : ",\"" IOTHUB_APP_PREFIX) == 0) &&
                (STRING_concat(existing, keys[i]) == 0) &&
                (STRING_concat(existing, "\":\"") == 0) &&
                (STRING_concat(existing, values[i]) == 0) &&
                (STRING_concat(existing, "\"") == 0)
                ))
            {
                LogError("unable to STRING_concat");
                break;
            }
        }

        if (i < count)
        {
            result = MU_FAILURE;
            /*error, let it go through*/
        }
        else if (STRING_concat(existing, "}") != 0)
        {
            LogError("unable to STRING_concat");
            result = MU_FAILURE;
        }
        else
        {
            /*all is fine*/
            result = 0;
        }
    }
    return result;
}

/*makes the following string:{"body":"base64 encoding of the message content"[,"properties":{"a":"valueOfA"}]}*/
/*return NULL if there was a failure, or a non-NULL STRING_HANDLE that contains the intended data*/
static STRING_HANDLE make1EventJSONitem(PDLIST_ENTRY item, size_t *messageSizeContribution)
{
    STRING_HANDLE result; /*temp wants to contain :{"body":"base64 encoding of the message content"[,"properties":{"a":"valueOfA"}]}*/
    IOTHUB_MESSAGE_LIST* message = containingRecord(item, IOTHUB_MESSAGE_LIST, entry);
    IOTHUBMESSAGE_CONTENT_TYPE contentType = IoTHubMessage_GetContentType(message->messageHandle);

    switch (contentType)
    {
    case IOTHUBMESSAGE_BYTEARRAY:
    {
        result = STRING_construct("{\"body\":\"");
        if (result == NULL)
        {
            LogError("unable to STRING_construct");
        }
        else
        {
            const unsigned char* source;
            size_t size;

            if (IoTHubMessage_GetByteArray(message->messageHandle, &source, &size) != IOTHUB_MESSAGE_OK)
            {
                LogError("unable to get the data for the message.");
                STRING_delete(result);
                result = NULL;
            }
            else
            {
                STRING_HANDLE encoded = Azure_Base64_Encode_Bytes(source, size);
                if (encoded == NULL)
                {
                    LogError("unable to Azure_Base64_Encode_Bytes.");
                    STRING_delete(result);
                    result = NULL;
                }
                else
                {
                    size_t propertiesSize = 0;
                    if (!(
                        (STRING_concat_with_STRING(result, encoded) == 0) &&
                        (STRING_concat(result, "\"") == 0) && /*\" because closing value*/
                        (concat_Properties(result, IoTHubMessage_Properties(message->messageHandle), &propertiesSize) == 0) &&
                        (STRING_concat(result, "},") == 0) /*the last comma shall be replaced by a ']' by DaCr's suggestion (which is awesome enough to receive credits in the source code)*/
                        ))
                    {
                        STRING_delete(result);
                        result = NULL;
                        LogError("unable to STRING_concat_with_STRING.");
                    }
                    else
                    {
                        /*all is fine... */
                        /*Codes_SRS_TRANSPORTMULTITHTTP_17_062: [The message size is computed from the length of the payload + 384.] */
                        *messageSizeContribution = size + MAXIMUM_PAYLOAD_OVERHEAD + propertiesSize;
                    }
                    STRING_delete(encoded);
                }
            }
        }
        break;
    }
    /*Codes_SRS_TRANSPORTMULTITHTTP_17_057: [If a messages to be send has type IOTHUBMESSAGE_STRING, then its serialization shall be {"body":"JSON encoding of the string", "base64Encoded":false}] */
    case IOTHUBMESSAGE_STRING:
    {
        result = STRING_construct("{\"body\":");
        if (result == NULL)
        {
            LogError("unable to STRING_construct");
        }
        else
        {
            const char* source = IoTHubMessage_GetString(message->messageHandle);
            if (source == NULL)
            {
                LogError("unable to IoTHubMessage_GetString");
                STRING_delete(result);
                result = NULL;
            }
            else
            {
                STRING_HANDLE asJson = STRING_new_JSON(source);
                if (asJson == NULL)
                {
                    LogError("unable to STRING_new_JSON");
                    STRING_delete(result);
                    result = NULL;
                }
                else
                {
                    size_t propertiesSize = 0;
                    if (!(
                        (STRING_concat_with_STRING(result, asJson) == 0) &&
                        (STRING_concat(result, ",\"base64Encoded\":false") == 0) &&
                        (concat_Properties(result, IoTHubMessage_Properties(message->messageHandle), &propertiesSize) == 0) &&
                        (STRING_concat(result, "},") == 0) /*the last comma shall be replaced by a ']' by DaCr's suggestion (which is awesome enough to receive credits in the source code)*/
                        ))
                    {
                        LogError("unable to STRING_concat_with_STRING");
                        STRING_delete(result);
                        result = NULL;
                    }
                    else
                    {
                        /*result has the intended content*/
                        /*Codes_SRS_TRANSPORTMULTITHTTP_17_062: [The message size is computed from the length of the payload + 384.] */
                        *messageSizeContribution = strlen(source) + MAXIMUM_PAYLOAD_OVERHEAD + propertiesSize;
                    }
                    STRING_delete(asJson);
                }
            }
        }
        break;
    }
    default:
    {
        LogError("an unknown message type was encountered (%d)", contentType);
        result = NULL; /*unknown message type*/
        break;
    }
    }
    return result;
}

#define MAKE_PAYLOAD_RESULT_VALUES \
    MAKE_PAYLOAD_OK, /*returned when there is a payload to be later send by HTTP*/ \
    MAKE_PAYLOAD_NO_ITEMS, /*returned when there are no items to be send*/ \
    MAKE_PAYLOAD_ERROR, /*returned when there were errors*/ \
    MAKE_PAYLOAD_FIRST_ITEM_DOES_NOT_FIT /*returned when the first item doesn't fit*/

MU_DEFINE_ENUM(MAKE_PAYLOAD_RESULT, MAKE_PAYLOAD_RESULT_VALUES);

/*this function assembles several {"body":"base64 encoding of the message content"," base64Encoded": true} into 1 payload*/
/*Codes_SRS_TRANSPORTMULTITHTTP_17_056: [IoTHubTransportHttp_DoWork shall build the following string:[{"body":"base64 encoding of the message1 content"},{"body":"base64 encoding of the message2 content"}...]]*/
static MAKE_PAYLOAD_RESULT makePayload(HTTPTRANSPORT_PERDEVICE_DATA* deviceData, STRING_HANDLE* payload)
{
    MAKE_PAYLOAD_RESULT result;
    size_t allMessagesSize = 0;
    *payload = STRING_construct("[");
    if (*payload == NULL)
    {
        LogError("unable to STRING_construct");
        result = MAKE_PAYLOAD_ERROR;
    }
    else
    {
        bool isFirst = true;
        PDLIST_ENTRY actual;
        bool keepGoing = true; /*keepGoing gets sometimes to false from within the loop*/
                               /*either all the items enter the list or only some*/
        result = MAKE_PAYLOAD_OK; /*optimistically initializing it*/
        while (keepGoing && ((actual = deviceData->waitingToSend->Flink) != deviceData->waitingToSend))
        {
            size_t messageSize;
            STRING_HANDLE temp = make1EventJSONitem(actual, &messageSize);
            if (isFirst)
            {
                isFirst = false;
                /*Codes_SRS_TRANSPORTMULTITHTTP_17_067: [If there is no valid payload, IoTHubTransportHttp_DoWork shall advance to the next activity.]*/
                if (temp == NULL) /*first item failed to create, nothing to send*/
                {
                    result = MAKE_PAYLOAD_ERROR;
                    STRING_delete(*payload);
                    *payload = NULL;
                    keepGoing = false;
                }
                else
                {
                    /*Codes_SRS_TRANSPORTMULTITHTTP_17_065: [If the oldest message in waitingToSend causes the message size to exceed the message size limit then it shall be removed from waitingToSend, and IoTHubClientCore_LL_SendComplete shall be called. Parameter PDLIST_ENTRY completed shall point to a list containing only the oldest item, and parameter IOTHUB_CLIENT_CONFIRMATION_RESULT result shall be set to IOTHUB_CLIENT_CONFIRMATION_BATCHSTATE_FAILED.]*/
                    /*Codes_SRS_TRANSPORTMULTITHTTP_17_061: [The message size shall be limited to 255KB - 1 byte.]*/
                    if (messageSize > MAXIMUM_MESSAGE_SIZE)
                    {
                        PDLIST_ENTRY head = DList_RemoveHeadList(deviceData->waitingToSend); /*actually this is the same as "actual", but now it is removed*/
                        DList_InsertTailList(&(deviceData->eventConfirmations), head);
                        result = MAKE_PAYLOAD_FIRST_ITEM_DOES_NOT_FIT;
                        STRING_delete(*payload);
                        *payload = NULL;
                        keepGoing = false;
                    }
                    else
                    {
                        if (STRING_concat_with_STRING(*payload, temp) != 0)
                        {
                            /*Codes_SRS_TRANSPORTMULTITHTTP_17_067: [If there is no valid payload, IoTHubTransportHttp_DoWork shall advance to the next activity.]*/
                            result = MAKE_PAYLOAD_ERROR;
                            STRING_delete(*payload);
                            *payload = NULL;
                            keepGoing = false;
                        }
                        else
                        {
                            /*first item was put nicely in the payload*/
                            PDLIST_ENTRY head = DList_RemoveHeadList(deviceData->waitingToSend); /*actually this is the same as "actual", but now it is removed*/
                            DList_InsertTailList(&(deviceData->eventConfirmations), head);
                            allMessagesSize += messageSize;
                        }
                    }
                    STRING_delete(temp);
                }
            }
            else
            {
                /*there is at least 1 item already in the payload*/
                if (temp == NULL)
                {
                    /*there are multiple payloads encoded, the last one had an internal error, just go with those - closing the payload happens "after the loop"*/
                    /*Codes_SRS_TRANSPORTMULTITHTTP_17_066: [If at any point during construction of the string there are errors, IoTHubTransportHttp_DoWork shall use the so far constructed string as payload.]*/
                    result = MAKE_PAYLOAD_OK;
                    keepGoing = false;
                }
                else
                {
                    if (allMessagesSize + messageSize > MAXIMUM_MESSAGE_SIZE)
                    {
                        /*this item doesn't make it to the payload, but the payload is valid so far*/
                        /*Codes_SRS_TRANSPORTMULTITHTTP_17_066: [If at any point during construction of the string there are errors, IoTHubTransportHttp_DoWork shall use the so far constructed string as payload.]*/
                        result = MAKE_PAYLOAD_OK;
                        keepGoing = false;
                    }
                    else if (STRING_concat_with_STRING(*payload, temp) != 0)
                    {
                        /*should still send what there is so far...*/
                        /*Codes_SRS_TRANSPORTMULTITHTTP_17_066: [If at any point during construction of the string there are errors, IoTHubTransportHttp_DoWork shall use the so far constructed string as payload.]*/
                        result = MAKE_PAYLOAD_OK;
                        keepGoing = false;
                    }
                    else
                    {
                        /*cool, the payload made it there, let's continue... */
                        PDLIST_ENTRY head = DList_RemoveHeadList(deviceData->waitingToSend); /*actually this is the same as "actual", but now it is removed*/
                        DList_InsertTailList(&(deviceData->eventConfirmations), head);
                        allMessagesSize += messageSize;
                    }
                    STRING_delete(temp);
                }
            }
        }

        /*closing the payload*/
        if (result == MAKE_PAYLOAD_OK)
        {
            ((char*)STRING_c_str(*payload))[STRING_length(*payload) - 1] = ']'; /*TODO - do this in STRING_HANDLE*/
        }
        else
        {
            /*no need to close anything*/
        }
    }
    return result;
}

static void reversePutListBackIn(PDLIST_ENTRY source, PDLIST_ENTRY destination)
{
    /*this function takes a list, and inserts it in another list. When done in the context of this file, it reverses the effects of a not-able-to-send situation*/
    DList_AppendTailList(destination->Flink, source);
    DList_RemoveEntryList(source);
    DList_InitializeListHead(source);
}

static void DoEvent(HTTPTRANSPORT_HANDLE_DATA* handleData, HTTPTRANSPORT_PERDEVICE_DATA* deviceData)
{

    if (DList_IsListEmpty(deviceData->waitingToSend))
    {
        /*Codes_SRS_TRANSPORTMULTITHTTP_17_060: [If the list is empty then IoTHubTransportHttp_DoWork shall proceed to the following action.] */
    }
    else
    {
        /*Codes_SRS_TRANSPORTMULTITHTTP_17_053: [If option SetBatching is true then _Dowork shall send batched event message as specced below.] */
        if (handleData->doBatchedTransfers)
        {
            /*Codes_SRS_TRANSPORTMULTITHTTP_17_054: [Request HTTP headers shall have the value of "Content-Type" created or updated to "application/vnd.microsoft.iothub.json" by a call to HTTPHeaders_ReplaceHeaderNameValuePair.] */
            if (HTTPHeaders_ReplaceHeaderNameValuePair(deviceData->eventHTTPrequestHeaders, CONTENT_TYPE, APPLICATION_VND_MICROSOFT_IOTHUB_JSON) != HTTP_HEADERS_OK)
            {
                /*Codes_SRS_TRANSPORTMULTITHTTP_17_055: [If updating Content-Type fails for any reason, then _DoWork shall advance to the next action.] */
                LogError("unable to HTTPHeaders_ReplaceHeaderNameValuePair");
            }
            else
            {
                /*Codes_SRS_TRANSPORTMULTITHTTP_17_059: [It shall inspect the "waitingToSend" DLIST passed in config structure.] */
                STRING_HANDLE payload;
                switch (makePayload(deviceData, &payload))
                {
                case MAKE_PAYLOAD_OK:
                {
                    /*Codes_SRS_TRANSPORTMULTITHTTP_17_068: [Once a final payload has been obtained, IoTHubTransportHttp_DoWork shall call HTTPAPIEX_SAS_ExecuteRequest passing the following parameters:] */
                    BUFFER_HANDLE temp = BUFFER_new();
                    if (temp == NULL)
                    {
                        LogError("unable to BUFFER_new");
                        /*Codes_SRS_TRANSPORTMULTITHTTP_17_067: [If there is no valid payload, IoTHubTransportHttp_DoWork shall advance to the next activity.]*/
                        reversePutListBackIn(&(deviceData->eventConfirmations), deviceData->waitingToSend);
                    }
                    else
                    {
                        if (BUFFER_build(temp, (const unsigned char*)STRING_c_str(payload), STRING_length(payload)) != 0)
                        {
                            LogError("unable to BUFFER_build");
                            //items go back to waitingToSend
                            /*Codes_SRS_TRANSPORTMULTITHTTP_17_067: [If there is no valid payload, IoTHubTransportHttp_DoWork shall advance to the next activity.]*/
                            reversePutListBackIn(&(deviceData->eventConfirmations), deviceData->waitingToSend);
                        }
                        else
                        {
                            unsigned int statusCode;
                            if (HTTPAPIEX_SAS_ExecuteRequest(
                                deviceData->sasObject,
                                handleData->httpApiExHandle,
                                HTTPAPI_REQUEST_POST,
                                STRING_c_str(deviceData->eventHTTPrelativePath),
                                deviceData->eventHTTPrequestHeaders,
                                temp,
                                &statusCode,
                                NULL,
                                NULL
                            ) != HTTPAPIEX_OK)
                            {
                                LogError("unable to HTTPAPIEX_ExecuteRequest");
                                //items go back to waitingToSend
                                /*Codes_SRS_TRANSPORTMULTITHTTP_17_069: [if HTTPAPIEX_SAS_ExecuteRequest fails or the http status code >=300 then IoTHubTransportHttp_DoWork shall not do any other action (it is assumed at the next _DoWork it shall be retried).] */
                                reversePutListBackIn(&(deviceData->eventConfirmations), deviceData->waitingToSend);
                            }
                            else
                            {
                                if (statusCode < 300)
                                {
                                    /*Codes_SRS_TRANSPORTMULTITHTTP_17_070: [If HTTPAPIEX_SAS_ExecuteRequest does not fail and http status code <300 then IoTHubTransportHttp_DoWork shall call IoTHubClientCore_LL_SendComplete. Parameter PDLIST_ENTRY completed shall point to a list containing all the items batched, and parameter IOTHUB_CLIENT_CONFIRMATION_RESULT result shall be set to IOTHUB_CLIENT_CONFIRMATION_OK. The batched items shall be removed from waitingToSend.] */
                                    handleData->transport_callbacks.send_complete_cb(&(deviceData->eventConfirmations), IOTHUB_CLIENT_CONFIRMATION_OK, deviceData->device_transport_ctx);
                                }
                                else
                                {
                                    //items go back to waitingToSend
                                    /*Codes_SRS_TRANSPORTMULTITHTTP_17_069: [if HTTPAPIEX_SAS_ExecuteRequest fails or the http status code >=300 then IoTHubTransportHttp_DoWork shall not do any other action (it is assumed at the next _DoWork it shall be retried).] */
                                    LogError("unexpected HTTP status code (%u)", statusCode);
                                    reversePutListBackIn(&(deviceData->eventConfirmations), deviceData->waitingToSend);
                                }
                            }
                        }
                        BUFFER_delete(temp);
                    }
                    STRING_delete(payload);
                    break;
                }
                case MAKE_PAYLOAD_FIRST_ITEM_DOES_NOT_FIT:
                {
                    handleData->transport_callbacks.send_complete_cb(&(deviceData->eventConfirmations), IOTHUB_CLIENT_CONFIRMATION_ERROR, deviceData->device_transport_ctx); // takes care of emptying the list too
                    break;
                }
                case MAKE_PAYLOAD_ERROR:
                {
                    LogError("unrecoverable errors while building a batch message");
                    break;
                }
                case MAKE_PAYLOAD_NO_ITEMS:
                {
                    /*do nothing*/
                    break;
                }
                default:
                {
                    LogError("internal error: switch's default branch reached when never intended");
                    break;
                }
                }
            }
        }
        else
        {
            const unsigned char* messageContent = NULL;
            size_t messageSize = 0;
            size_t originalMessageSize = 0;
            IOTHUB_MESSAGE_LIST* message = containingRecord(deviceData->waitingToSend->Flink, IOTHUB_MESSAGE_LIST, entry);
            IOTHUBMESSAGE_CONTENT_TYPE contentType = IoTHubMessage_GetContentType(message->messageHandle);

            /*Codes_SRS_TRANSPORTMULTITHTTP_17_073: [The message size is computed from the length of the payload + 384.]*/
            if (!(
                (((contentType == IOTHUBMESSAGE_BYTEARRAY) &&
                (IoTHubMessage_GetByteArray(message->messageHandle, &messageContent, &originalMessageSize) == IOTHUB_MESSAGE_OK))
                    ? ((void)(messageSize = originalMessageSize + MAXIMUM_PAYLOAD_OVERHEAD), 1)
                    : 0)

                ||

                ((contentType == IOTHUBMESSAGE_STRING) &&
                ((void)(messageContent = (const unsigned char*)IoTHubMessage_GetString(message->messageHandle)),
                    ((void)(messageSize = MAXIMUM_PAYLOAD_OVERHEAD + (originalMessageSize = ((messageContent == NULL)
                        ? 0
                        : strlen((const char*)messageContent))))),
                    messageContent != NULL)
                    )
                ))
            {
                LogError("unable to get the message content");
                /*go on...*/
            }
            else
            {
                /*Codes_SRS_TRANSPORTMULTITHTTP_17_075: [If the oldest message in waitingToSend causes the message to exceed the message size limit then it shall be removed from waitingToSend, and IoTHubClientCore_LL_SendComplete shall be called. Parameter PDLIST_ENTRY completed shall point to a list containing only the oldest item, and parameter IOTHUB_CLIENT_CONFIRMATION_RESULT result shall be set to IOTHUB_CLIENT_CONFIRMATION_BATCHSTATE_FAILED.]*/
                /*Codes_SRS_TRANSPORTMULTITHTTP_17_072: [The message size shall be limited to 255KB -1 bytes.] */
                if (messageSize > MAXIMUM_MESSAGE_SIZE)
                {
                    PDLIST_ENTRY head = DList_RemoveHeadList(deviceData->waitingToSend); /*actually this is the same as "actual", but now it is removed*/
                    DList_InsertTailList(&(deviceData->eventConfirmations), head);
                    handleData->transport_callbacks.send_complete_cb(&(deviceData->eventConfirmations), IOTHUB_CLIENT_CONFIRMATION_ERROR, deviceData->device_transport_ctx); // takes care of emptying the list too
                }
                else
                {
                    /*Codes_SRS_TRANSPORTMULTITHTTP_17_071: [If option SetBatching is false then _Dowork shall send individual event message as specced below.] */
                    /*Codes_SRS_TRANSPORTMULTITHTTP_17_076: [A clone of the event HTTP request headers shall be created.]*/
                    HTTP_HEADERS_HANDLE clonedEventHTTPrequestHeaders = HTTPHeaders_Clone(deviceData->eventHTTPrequestHeaders);
                    if (clonedEventHTTPrequestHeaders == NULL)
                    {
                        /*Codes_SRS_TRANSPORTMULTITHTTP_17_079: [If any HTTP header operation fails, _DoWork shall advance to the next action.] */
                        LogError("HTTPHeaders_Clone failed");
                    }
                    else
                    {
                        /*Codes_SRS_TRANSPORTMULTITHTTP_17_077: [The cloned HTTP headers shall have the HTTP header "Content-Type" set to "application/octet-stream".] */
                        if (HTTPHeaders_ReplaceHeaderNameValuePair(clonedEventHTTPrequestHeaders, CONTENT_TYPE, APPLICATION_OCTET_STREAM) != HTTP_HEADERS_OK)
                        {
                            /*Codes_SRS_TRANSPORTMULTITHTTP_17_079: [If any HTTP header operation fails, _DoWork shall advance to the next action.] */
                            LogError("HTTPHeaders_ReplaceHeaderNameValuePair failed");
                        }
                        else
                        {
                            // set_message_properties returning false does not necessarily mean the the function failed, it just means
                            // the the adding of messages should not continue and should try the next time.  So you should not log if this
                            // returns false
                            if (set_message_properties(message, &messageSize, clonedEventHTTPrequestHeaders, handleData, deviceData))
                            {

                                if (set_system_properties(message, clonedEventHTTPrequestHeaders) != 0)
                                {
                                    /*Codes_SRS_TRANSPORTMULTITHTTP_17_079: [If any HTTP header operation fails, _DoWork shall advance to the next action.] */
                                }
                                else
                                {
                                    BUFFER_HANDLE toBeSend = BUFFER_create(messageContent, originalMessageSize);
                                    if (toBeSend == NULL)
                                    {
                                        LogError("unable to BUFFER_new");
                                    }
                                    else
                                    {
                                        unsigned int statusCode = 0;
                                        HTTPAPIEX_RESULT r;
                                        if (deviceData->deviceSasToken != NULL)
                                        {
                                            /*Codes_SRS_TRANSPORTMULTITHTTP_03_001: [if a deviceSasToken exists, HTTPHeaders_ReplaceHeaderNameValuePair shall be invoked with "Authorization" as its second argument and STRING_c_str (deviceSasToken) as its third argument.]*/
                                            if (HTTPHeaders_ReplaceHeaderNameValuePair(clonedEventHTTPrequestHeaders, IOTHUB_AUTH_HEADER_VALUE, STRING_c_str(deviceData->deviceSasToken)) != HTTP_HEADERS_OK)
                                            {
                                                r = HTTPAPIEX_ERROR;
                                                /*Codes_SRS_TRANSPORTMULTITHTTP_03_002: [If the result of the invocation of HTTPHeaders_ReplaceHeaderNameValuePair is NOT HTTP_HEADERS_OK then fallthrough.]*/
                                                LogError("Unable to replace the old SAS Token.");
                                            }
                                            /*Codes_SRS_TRANSPORTMULTITHTTP_03_003: [If a deviceSasToken exists, IoTHubTransportHttp_DoWork shall call HTTPAPIEX_ExecuteRequest passing the following parameters] */
                                            else if ((r = HTTPAPIEX_ExecuteRequest(
                                                handleData->httpApiExHandle, HTTPAPI_REQUEST_POST, STRING_c_str(deviceData->eventHTTPrelativePath),
                                                clonedEventHTTPrequestHeaders, toBeSend, &statusCode, NULL, NULL)) != HTTPAPIEX_OK)
                                            {
                                                LogError("Unable to HTTPAPIEX_ExecuteRequest.");
                                            }
                                        }
                                        else
                                        {
                                            /*Codes_SRS_TRANSPORTMULTITHTTP_17_080: [If a deviceSasToken does not exist, IoTHubTransportHttp_DoWork shall call HTTPAPIEX_SAS_ExecuteRequest passing the following parameters] */
                                            if ((r = HTTPAPIEX_SAS_ExecuteRequest(deviceData->sasObject, handleData->httpApiExHandle, HTTPAPI_REQUEST_POST, STRING_c_str(deviceData->eventHTTPrelativePath),
                                                clonedEventHTTPrequestHeaders, toBeSend, &statusCode, NULL, NULL )) != HTTPAPIEX_OK)
                                            {
                                                LogError("unable to HTTPAPIEX_SAS_ExecuteRequest");
                                            }
                                        }
                                        if (r == HTTPAPIEX_OK)
                                        {
                                            if (statusCode < 300)
                                            {
                                                /*Codes_SRS_TRANSPORTMULTITHTTP_17_082: [If HTTPAPIEX_SAS_ExecuteRequest does not fail and http status code <300 then IoTHubTransportHttp_DoWork shall call IoTHubClientCore_LL_SendComplete. Parameter PDLIST_ENTRY completed shall point to a list the item send, and parameter IOTHUB_CLIENT_CONFIRMATION_RESULT result shall be set to IOTHUB_CLIENT_CONFIRMATION_OK. The item shall be removed from waitingToSend.] */
                                                PDLIST_ENTRY justSent = DList_RemoveHeadList(deviceData->waitingToSend); /*actually this is the same as "actual", but now it is removed*/
                                                DList_InsertTailList(&(deviceData->eventConfirmations), justSent);
                                                handleData->transport_callbacks.send_complete_cb(&(deviceData->eventConfirmations), IOTHUB_CLIENT_CONFIRMATION_OK, deviceData->device_transport_ctx); // takes care of emptying the list too
                                            }
                                            else
                                            {
                                                /*Codes_SRS_TRANSPORTMULTITHTTP_17_081: [If HTTPAPIEX_SAS_ExecuteRequest fails or the http status code >=300 then IoTHubTransportHttp_DoWork shall not do any other action (it is assumed at the next _DoWork it shall be retried).] */
                                                LogError("unexpected HTTP status code (%u)", statusCode);
                                            }
                                        }
                                        else if (r == HTTPAPIEX_RECOVERYFAILED)
                                        {
                                            PDLIST_ENTRY justSent = DList_RemoveHeadList(deviceData->waitingToSend); /*actually this is the same as "actual", but now it is removed*/
                                            DList_InsertTailList(&(deviceData->eventConfirmations), justSent);
                                            handleData->transport_callbacks.send_complete_cb(&(deviceData->eventConfirmations), IOTHUB_CLIENT_CONFIRMATION_ERROR, deviceData->device_transport_ctx); // takes care of emptying the list too
                                        }
                                    }
                                    BUFFER_delete(toBeSend);
                                }
                            }
                        }
                        HTTPHeaders_Free(clonedEventHTTPrequestHeaders);
                    }
                }
            }
        }
    }
}

static bool abandonOrAcceptMessage(HTTPTRANSPORT_HANDLE_DATA* handleData, HTTPTRANSPORT_PERDEVICE_DATA* deviceData, const char* ETag, IOTHUBMESSAGE_DISPOSITION_RESULT action)
{
    /*Codes_SRS_TRANSPORTMULTITHTTP_17_097: [_DoWork shall call HTTPAPIEX_SAS_ExecuteRequest with the following parameters:
    -requestType: POST
    -relativePath: abandon relative path begin (as created by _Create) + value of ETag + "/abandon?api-version=2016-11-14"
    - requestHttpHeadersHandle: an HTTP headers instance containing the following
    Authorization: " "
    If-Match: value of ETag
    - requestContent: NULL
    - statusCode: a pointer to unsigned int which might be examined for logging
    - responseHeadearsHandle: NULL
    - responseContent: NULL]*/
    /*Codes_SRS_TRANSPORTMULTITHTTP_17_099: [_DoWork shall call HTTPAPIEX_SAS_ExecuteRequest with the following parameters:
    -requestType: DELETE
    -relativePath: abandon relative path begin + value of ETag + "?api-version=2016-11-14"
    - requestHttpHeadersHandle: an HTTP headers instance containing the following
    Authorization: " "
    If-Match: value of ETag
    - requestContent: NULL
    - statusCode: a pointer to unsigned int which might be used by logging
    - responseHeadearsHandle: NULL
    - responseContent: NULL]*/
    /*Codes_SRS_TRANSPORTMULTITHTTP_17_101: [_DoWork shall call HTTPAPIEX_SAS_ExecuteRequest with the following parameters:
    -requestType: DELETE
    -relativePath: abandon relative path begin + value of ETag +"?api-version=2016-11-14" + "&reject"
    - requestHttpHeadersHandle: an HTTP headers instance containing the following
    Authorization: " "
    If-Match: value of ETag
    - requestContent: NULL
    - statusCode: a pointer to unsigned int which might be used by logging
    - responseHeadearsHandle: NULL
    - responseContent: NULL]*/

    bool result;
    STRING_HANDLE fullAbandonRelativePath = STRING_clone(deviceData->abandonHTTPrelativePathBegin);
    if (fullAbandonRelativePath == NULL)
    {
        /*Codes_SRS_TRANSPORTMULTITHTTP_17_098: [Abandoning the message is considered successful if the HTTPAPIEX_SAS_ExecuteRequest doesn't fail and the statusCode is 204.]*/
        /*Codes_SRS_TRANSPORTMULTITHTTP_17_100: [Accepting a message is successful when HTTPAPIEX_SAS_ExecuteRequest completes successfully and the status code is 204.] */
        /*Codes_SRS_TRANSPORTMULTITHTTP_17_102: [Rejecting a message is successful when HTTPAPIEX_SAS_ExecuteRequest completes successfully and the status code is 204.] */
        LogError("unable to STRING_clone");
        result = false;
    }
    else
    {
        STRING_HANDLE ETagUnquoted = STRING_construct_n(ETag + 1, strlen(ETag) - 2); /*skip first character which is '"' and the last one (which is also '"')*/
        if (ETagUnquoted == NULL)
        {
            /*Codes_SRS_TRANSPORTMULTITHTTP_17_098: [Abandoning the message is considered successful if the HTTPAPIEX_SAS_ExecuteRequest doesn't fail and the statusCode is 204.]*/
            /*Codes_SRS_TRANSPORTMULTITHTTP_17_100: [Accepting a message is successful when HTTPAPIEX_SAS_ExecuteRequest completes successfully and the status code is 204.] */
            /*Codes_SRS_TRANSPORTMULTITHTTP_17_102: [Rejecting a message is successful when HTTPAPIEX_SAS_ExecuteRequest completes successfully and the status code is 204.] */
            LogError("unable to STRING_construct_n");
            result = false;
        }
        else
        {
            if (!(
                (STRING_concat_with_STRING(fullAbandonRelativePath, ETagUnquoted) == 0) &&
                (STRING_concat(fullAbandonRelativePath, (action == IOTHUBMESSAGE_ABANDONED) ? "/abandon" API_VERSION : ((action == IOTHUBMESSAGE_REJECTED) ? API_VERSION "&reject" : API_VERSION)) == 0)
                ))
            {
                /*Codes_SRS_TRANSPORTMULTITHTTP_17_098: [Abandoning the message is considered successful if the HTTPAPIEX_SAS_ExecuteRequest doesn't fail and the statusCode is 204.]*/
                /*Codes_SRS_TRANSPORTMULTITHTTP_17_100: [Accepting a message is successful when HTTPAPIEX_SAS_ExecuteRequest completes successfully and the status code is 204.] */
                /*Codes_SRS_TRANSPORTMULTITHTTP_17_102: [Rejecting a message is successful when HTTPAPIEX_SAS_ExecuteRequest completes successfully and the status code is 204.] */
                LogError("unable to STRING_concat");
                result = false;
            }
            else
            {
                HTTP_HEADERS_HANDLE abandonRequestHttpHeaders = HTTPHeaders_Alloc();
                if (abandonRequestHttpHeaders == NULL)
                {
                    /*Codes_SRS_TRANSPORTMULTITHTTP_17_098: [Abandoning the message is considered successful if the HTTPAPIEX_SAS_ExecuteRequest doesn't fail and the statusCode is 204.]*/
                    /*Codes_SRS_TRANSPORTMULTITHTTP_17_100: [Accepting a message is successful when HTTPAPIEX_SAS_ExecuteRequest completes successfully and the status code is 204.] */
                    /*Codes_SRS_TRANSPORTMULTITHTTP_17_102: [Rejecting a message is successful when HTTPAPIEX_SAS_ExecuteRequest completes successfully and the status code is 204.] */
                    LogError("unable to HTTPHeaders_Alloc");
                    result = false;
                }
                else
                {
                    if (!(
                        (addUserAgentHeaderInfo(handleData, abandonRequestHttpHeaders) == HTTP_HEADERS_OK) &&
                        (HTTPHeaders_AddHeaderNameValuePair(abandonRequestHttpHeaders, IOTHUB_AUTH_HEADER_VALUE, " ") == HTTP_HEADERS_OK) &&
                        (HTTPHeaders_AddHeaderNameValuePair(abandonRequestHttpHeaders, "If-Match", ETag) == HTTP_HEADERS_OK)
                        ))
                    {
                        /*Codes_SRS_TRANSPORTMULTITHTTP_17_098: [Abandoning the message is considered successful if the HTTPAPIEX_SAS_ExecuteRequest doesn't fail and the statusCode is 204.]*/
                        /*Codes_SRS_TRANSPORTMULTITHTTP_17_100: [Accepting a message is successful when HTTPAPIEX_SAS_ExecuteRequest completes successfully and the status code is 204.] */
                        /*Codes_SRS_TRANSPORTMULTITHTTP_17_102: [Rejecting a message is successful when HTTPAPIEX_SAS_ExecuteRequest completes successfully and the status code is 204.] */
                        LogError("unable to HTTPHeaders_AddHeaderNameValuePair");
                        result = false;
                    }
                    else
                    {
                        unsigned int statusCode = 0;
                        HTTPAPIEX_RESULT r;
                        if (deviceData->deviceSasToken != NULL)
                        {
                            /*Codes_SRS_TRANSPORTMULTITHTTP_03_001: [if a deviceSasToken exists, HTTPHeaders_ReplaceHeaderNameValuePair shall be invoked with "Authorization" as its second argument and STRING_c_str (deviceSasToken) as its third argument.]*/
                            if (HTTPHeaders_ReplaceHeaderNameValuePair(abandonRequestHttpHeaders, IOTHUB_AUTH_HEADER_VALUE, STRING_c_str(deviceData->deviceSasToken)) != HTTP_HEADERS_OK)
                            {
                                r = HTTPAPIEX_ERROR;
                                /*Codes_SRS_TRANSPORTMULTITHTTP_03_002: [If the result of the invocation of HTTPHeaders_ReplaceHeaderNameValuePair is NOT HTTP_HEADERS_OK then fallthrough.]*/
                                LogError("Unable to replace the old SAS Token.");
                                result = false;
                            }
                            else if ((r = HTTPAPIEX_ExecuteRequest(
                                handleData->httpApiExHandle,
                                (action == IOTHUBMESSAGE_ABANDONED) ? HTTPAPI_REQUEST_POST : HTTPAPI_REQUEST_DELETE,                               /*-requestType: POST                                                                                                       */
                                STRING_c_str(fullAbandonRelativePath),              /*-relativePath: abandon relative path begin (as created by _Create) + value of ETag + "/abandon?api-version=2016-11-14"   */
                                abandonRequestHttpHeaders,                          /*- requestHttpHeadersHandle: an HTTP headers instance containing the following                                            */
                                NULL,                                               /*- requestContent: NULL                                                                                                   */
                                &statusCode,                                         /*- statusCode: a pointer to unsigned int which might be examined for logging                                              */
                                NULL,                                               /*- responseHeadearsHandle: NULL                                                                                           */
                                NULL                                                /*- responseContent: NULL]                                                                                                 */
                            )) != HTTPAPIEX_OK)
                            {
                                /*Codes_SRS_TRANSPORTMULTITHTTP_17_098: [Abandoning the message is considered successful if the HTTPAPIEX_SAS_ExecuteRequest doesn't fail and the statusCode is 204.]*/
                                /*Codes_SRS_TRANSPORTMULTITHTTP_17_100: [Accepting a message is successful when HTTPAPIEX_SAS_ExecuteRequest completes successfully and the status code is 204.] */
                                /*Codes_SRS_TRANSPORTMULTITHTTP_17_102: [Rejecting a message is successful when HTTPAPIEX_SAS_ExecuteRequest completes successfully and the status code is 204.] */
                                LogError("Unable to HTTPAPIEX_ExecuteRequest.");
                                result = false;
                            }
                        }
                        else if ((r = HTTPAPIEX_SAS_ExecuteRequest(
                            deviceData->sasObject,
                            handleData->httpApiExHandle,
                            (action == IOTHUBMESSAGE_ABANDONED) ? HTTPAPI_REQUEST_POST : HTTPAPI_REQUEST_DELETE,                               /*-requestType: POST                                                                                                       */
                            STRING_c_str(fullAbandonRelativePath),              /*-relativePath: abandon relative path begin (as created by _Create) + value of ETag + "/abandon?api-version=2016-11-14"   */
                            abandonRequestHttpHeaders,                          /*- requestHttpHeadersHandle: an HTTP headers instance containing the following                                            */
                            NULL,                                               /*- requestContent: NULL                                                                                                   */
                            &statusCode,                                         /*- statusCode: a pointer to unsigned int which might be examined for logging                                              */
                            NULL,                                               /*- responseHeadearsHandle: NULL                                                                                           */
                            NULL                                                /*- responseContent: NULL]                                                                                                 */
                        )) != HTTPAPIEX_OK)
                        {
                            /*Codes_SRS_TRANSPORTMULTITHTTP_17_098: [Abandoning the message is considered successful if the HTTPAPIEX_SAS_ExecuteRequest doesn't fail and the statusCode is 204.]*/
                            /*Codes_SRS_TRANSPORTMULTITHTTP_17_100: [Accepting a message is successful when HTTPAPIEX_SAS_ExecuteRequest completes successfully and the status code is 204.] */
                            /*Codes_SRS_TRANSPORTMULTITHTTP_17_102: [Rejecting a message is successful when HTTPAPIEX_SAS_ExecuteRequest completes successfully and the status code is 204.] */
                            LogError("unable to HTTPAPIEX_SAS_ExecuteRequest");
                            result = false;
                        }
                        if (r == HTTPAPIEX_OK)
                        {
                            if (statusCode != 204)
                            {
                                /*Codes_SRS_TRANSPORTMULTITHTTP_17_098: [Abandoning the message is considered successful if the HTTPAPIEX_SAS_ExecuteRequest doesn't fail and the statusCode is 204.]*/
                                /*Codes_SRS_TRANSPORTMULTITHTTP_17_100: [Accepting a message is successful when HTTPAPIEX_SAS_ExecuteRequest completes successfully and the status code is 204.] */
                                /*Codes_SRS_TRANSPORTMULTITHTTP_17_102: [Rejecting a message is successful when HTTPAPIEX_SAS_ExecuteRequest completes successfully and the status code is 204.] */
                                LogError("unexpected status code returned %u (was expecting 204)", statusCode);
                                result = false;
                            }
                            else
                            {
                                /*Codes_SRS_TRANSPORTMULTITHTTP_17_098: [Abandoning the message is considered successful if the HTTPAPIEX_SAS_ExecuteRequest doesn't fail and the statusCode is 204.]*/
                                /*Codes_SRS_TRANSPORTMULTITHTTP_17_100: [Accepting a message is successful when HTTPAPIEX_SAS_ExecuteRequest completes successfully and the status code is 204.] */
                                /*Codes_SRS_TRANSPORTMULTITHTTP_17_102: [Rejecting a message is successful when HTTPAPIEX_SAS_ExecuteRequest completes successfully and the status code is 204.] */
                                /*all is fine*/
                                result = true;
                            }
                        }
                        else
                        {
                            result = false;
                        }
                    }
                    HTTPHeaders_Free(abandonRequestHttpHeaders);
                }
            }
            STRING_delete(ETagUnquoted);
        }
        STRING_delete(fullAbandonRelativePath);
    }
    return result;
}

static IOTHUB_CLIENT_RESULT IoTHubTransportHttp_SendMessageDisposition(MESSAGE_CALLBACK_INFO* message_data, IOTHUBMESSAGE_DISPOSITION_RESULT disposition)
{
    IOTHUB_CLIENT_RESULT result;
    if (message_data == NULL)
    {
        /* Codes_SRS_TRANSPORTMULTITHTTP_10_001: [If messageData is NULL, IoTHubTransportHttp_SendMessageDisposition shall fail and return IOTHUB_CLIENT_ERROR.] */
        LogError("invalid argument messageData is NULL");
        result = IOTHUB_CLIENT_ERROR;
    }
    else
    {
        if (message_data->messageHandle == NULL)
        {
            /* Codes_SRS_TRANSPORTMULTITHTTP_10_002: [If any of the messageData fields are NULL, IoTHubTransportHttp_SendMessageDisposition shall fail and return IOTHUB_CLIENT_ERROR.] */
            LogError("invalid message handle");
            result = IOTHUB_CLIENT_ERROR;
        }
        else
        {
            MESSAGE_DISPOSITION_CONTEXT* tc = (MESSAGE_DISPOSITION_CONTEXT*)(message_data->transportContext);
            if (tc == NULL)
            {
                /* Codes_SRS_TRANSPORTMULTITHTTP_10_002: [If any of the messageData fields are NULL, IoTHubTransportHttp_SendMessageDisposition shall fail and return IOTHUB_CLIENT_ERROR.] */
                LogError("invalid transport context data");
                result = IOTHUB_CLIENT_ERROR;
            }
            else
            {
                if ((tc->handleData == NULL) || (tc->deviceData == NULL) || (tc->etagValue == NULL))
                {
                    /* Codes_SRS_TRANSPORTMULTITHTTP_10_002: [If any of the messageData fields are NULL, IoTHubTransportHttp_SendMessageDisposition shall fail and return IOTHUB_CLIENT_ERROR.] */
                    LogError("invalid transport context data");
                    result = IOTHUB_CLIENT_ERROR;
                }
                else
                {
                    if (abandonOrAcceptMessage(tc->handleData, tc->deviceData, tc->etagValue, disposition))
                    {
                        result = IOTHUB_CLIENT_OK;
                    }
                    else
                    {
                        /* Codes_SRS_TRANSPORTMULTITHTTP_10_003: [IoTHubTransportHttp_SendMessageDisposition shall fail and return IOTHUB_CLIENT_ERROR if the POST message fails, otherwise return IOTHUB_CLIENT_OK.] */
                        LogError("HTTP Transport layer failed to report %s disposition", MU_ENUM_TO_STRING(IOTHUBMESSAGE_DISPOSITION_RESULT, disposition));
                        result = IOTHUB_CLIENT_ERROR;
                    }
                }
                free(tc->etagValue);
                free(tc);
            }
            IoTHubMessage_Destroy(message_data->messageHandle);
        }
        free(message_data);
    }
    return result;
}

static MESSAGE_CALLBACK_INFO* MESSAGE_CALLBACK_INFO_Create(IOTHUB_MESSAGE_HANDLE received_message, HTTPTRANSPORT_HANDLE_DATA* handleData, HTTPTRANSPORT_PERDEVICE_DATA* deviceData, const char* etagValue)
{
    MESSAGE_CALLBACK_INFO* result = (MESSAGE_CALLBACK_INFO*)malloc(sizeof(MESSAGE_CALLBACK_INFO));
    if (result == NULL)
    {
        /*Codes_SRS_TRANSPORTMULTITHTTP_10_006: [If assembling the transport context fails, _DoWork shall "abandon" the message.] */
        LogError("malloc failed");
    }
    else
    {
        MESSAGE_DISPOSITION_CONTEXT* tc = (MESSAGE_DISPOSITION_CONTEXT*)malloc(sizeof(MESSAGE_DISPOSITION_CONTEXT));
        if (tc == NULL)
        {
            /*Codes_SRS_TRANSPORTMULTITHTTP_10_006: [If assembling the transport context fails, _DoWork shall "abandon" the message.] */
            LogError("malloc failed");
            free(result);
            result = NULL;
        }
        else
        {
            result->messageHandle = IoTHubMessage_Clone(received_message);
            if (result->messageHandle == NULL)
            {
                /*Codes_SRS_TRANSPORTMULTITHTTP_10_007: [If assembling a message clone, _DoWork shall "abandon" the message.]*/
                LogError("IoTHubMessage_Clone failed");
                free(tc);
                free(result);
                result = NULL;
            }
            else
            {
                if (mallocAndStrcpy_s(&tc->etagValue, etagValue) != 0)
                {
                    LogError("mallocAndStrcpy_s failed");
                    free(tc);
                    free(result);
                    result = NULL;
                }
                else
                {
                    tc->handleData = handleData;
                    tc->deviceData = deviceData;

                    result->transportContext = tc;
                }
            }
        }
    }
    return result;
}

static void DoMessages(HTTPTRANSPORT_HANDLE_DATA* handleData, HTTPTRANSPORT_PERDEVICE_DATA* deviceData)
{
    /*Codes_SRS_TRANSPORTMULTITHTTP_17_083: [ If device is not subscribed then _DoWork shall advance to the next action. ] */
    if (deviceData->DoWork_PullMessage)
    {
        /*Codes_SRS_TRANSPORTMULTITHTTP_17_123: [After client creation, the first GET shall be allowed no matter what the value of GetMinimumPollingTime.] */
        /*Codes_SRS_TRANSPORTMULTITHTTP_17_124: [If time is not available then all calls shall be treated as if they are the first one.] */
        /*Codes_SRS_TRANSPORTMULTITHTTP_17_122: [A GET request that happens earlier than GetMinimumPollingTime shall be ignored.] */
        time_t timeNow = get_time(NULL);
        bool isPollingAllowed = deviceData->isFirstPoll || (timeNow == (time_t)(-1)) || (get_difftime(timeNow, deviceData->lastPollTime) > handleData->getMinimumPollingTime);
        if (isPollingAllowed)
        {
            HTTP_HEADERS_HANDLE responseHTTPHeaders = HTTPHeaders_Alloc();
            if (responseHTTPHeaders == NULL)
            {
                /*Codes_SRS_TRANSPORTMULTITHTTP_17_085: [If the call to HTTPAPIEX_SAS_ExecuteRequest did not executed successfully or building any part of the prerequisites of the call fails, then _DoWork shall advance to the next action in this description.] */
                LogError("unable to HTTPHeaders_Alloc");
            }
            else
            {
                BUFFER_HANDLE responseContent = BUFFER_new();
                if (responseContent == NULL)
                {
                    /*Codes_SRS_TRANSPORTMULTITHTTP_17_085: [If the call to HTTPAPIEX_SAS_ExecuteRequest did not executed successfully or building any part of the prerequisites of the call fails, then _DoWork shall advance to the next action in this description.] */
                    LogError("unable to BUFFER_new");
                }
                else
                {
                    unsigned int statusCode = 0;
                    HTTPAPIEX_RESULT r;
                    if (deviceData->deviceSasToken != NULL)
                    {
                        /*Codes_SRS_TRANSPORTMULTITHTTP_03_001: [if a deviceSasToken exists, HTTPHeaders_ReplaceHeaderNameValuePair shall be invoked with "Authorization" as its second argument and STRING_c_str (deviceSasToken) as its third argument.]*/
                        if (HTTPHeaders_ReplaceHeaderNameValuePair(deviceData->messageHTTPrequestHeaders, IOTHUB_AUTH_HEADER_VALUE, STRING_c_str(deviceData->deviceSasToken)) != HTTP_HEADERS_OK)
                        {
                            r = HTTPAPIEX_ERROR;
                            /*Codes_SRS_TRANSPORTMULTITHTTP_03_002: [If the result of the invocation of HTTPHeaders_ReplaceHeaderNameValuePair is NOT HTTP_HEADERS_OK then fallthrough.]*/
                            LogError("Unable to replace the old SAS Token.");
                        }
                        else if ((r = HTTPAPIEX_ExecuteRequest(
                            handleData->httpApiExHandle,
                            HTTPAPI_REQUEST_GET,                                            /*requestType: GET*/
                            STRING_c_str(deviceData->messageHTTPrelativePath),         /*relativePath: the message HTTP relative path*/
                            deviceData->messageHTTPrequestHeaders,                     /*requestHttpHeadersHandle: message HTTP request headers created by _Create*/
                            NULL,                                                           /*requestContent: NULL*/
                            &statusCode,                                                    /*statusCode: a pointer to unsigned int which shall be later examined*/
                            responseHTTPHeaders,                                            /*responseHeadearsHandle: a new instance of HTTP headers*/
                            responseContent                                                 /*responseContent: a new instance of buffer*/
                        )) != HTTPAPIEX_OK)
                        {
                            /*Codes_SRS_TRANSPORTMULTITHTTP_17_085: [If the call to HTTPAPIEX_SAS_ExecuteRequest did not executed successfully or building any part of the prerequisites of the call fails, then _DoWork shall advance to the next action in this description.] */
                            LogError("Unable to HTTPAPIEX_ExecuteRequest.");
                        }
                    }

                    /*Codes_SRS_TRANSPORTMULTITHTTP_17_084: [Otherwise, IoTHubTransportHttp_DoWork shall call HTTPAPIEX_SAS_ExecuteRequest passing the following parameters
                    requestType: GET
                    relativePath: the message HTTP relative path
                    requestHttpHeadersHandle: message HTTP request headers created by _Create
                    requestContent: NULL
                    statusCode: a pointer to unsigned int which shall be later examined
                    responseHeadearsHandle: a new instance of HTTP headers
                    responseContent: a new instance of buffer]
                    */
                    else if ((r = HTTPAPIEX_SAS_ExecuteRequest(
                        deviceData->sasObject,
                        handleData->httpApiExHandle,
                        HTTPAPI_REQUEST_GET,                                            /*requestType: GET*/
                        STRING_c_str(deviceData->messageHTTPrelativePath),         /*relativePath: the message HTTP relative path*/
                        deviceData->messageHTTPrequestHeaders,                     /*requestHttpHeadersHandle: message HTTP request headers created by _Create*/
                        NULL,                                                           /*requestContent: NULL*/
                        &statusCode,                                                    /*statusCode: a pointer to unsigned int which shall be later examined*/
                        responseHTTPHeaders,                                            /*responseHeadearsHandle: a new instance of HTTP headers*/
                        responseContent                                                 /*responseContent: a new instance of buffer*/
                    )) != HTTPAPIEX_OK)
                    {
                        /*Codes_SRS_TRANSPORTMULTITHTTP_17_085: [If the call to HTTPAPIEX_SAS_ExecuteRequest did not executed successfully or building any part of the prerequisites of the call fails, then _DoWork shall advance to the next action in this description.] */
                        LogError("unable to HTTPAPIEX_SAS_ExecuteRequest");
                    }
                    if (r == HTTPAPIEX_OK)
                    {
                        /*HTTP dialogue was succesfull*/
                        if (timeNow == (time_t)(-1))
                        {
                            deviceData->isFirstPoll = true;
                        }
                        else
                        {
                            deviceData->isFirstPoll = false;
                            deviceData->lastPollTime = timeNow;
                        }
                        if (statusCode == 204)
                        {
                            /*Codes_SRS_TRANSPORTMULTITHTTP_17_086: [If the HTTPAPIEX_SAS_ExecuteRequest executed successfully then status code shall be examined. Any status code different than 200 causes _DoWork to advance to the next action.] */
                            /*this is an expected status code, means "no commands", but logging that creates panic*/

                            /*do nothing, advance to next action*/
                        }
                        else if (statusCode != 200)
                        {
                            /*Codes_SRS_TRANSPORTMULTITHTTP_17_086: [If the HTTPAPIEX_SAS_ExecuteRequest executed successfully then status code shall be examined. Any status code different than 200 causes _DoWork to advance to the next action.] */
                            LogError("expected status code was 200, but actually was received %u... moving on", statusCode);
                        }
                        else
                        {
                            /*Codes_SRS_TRANSPORTMULTITHTTP_17_087: [If status code is 200, then _DoWork shall make a copy of the value of the "ETag" http header.]*/
                            const char* etagValue = HTTPHeaders_FindHeaderValue(responseHTTPHeaders, "ETag");
                            if (etagValue == NULL)
                            {
                                LogError("unable to find a received header called \"E-Tag\"");
                            }
                            else
                            {
                                /*Codes_SRS_TRANSPORTMULTITHTTP_17_088: [If no such header is found or is invalid, then _DoWork shall advance to the next action.]*/
                                size_t etagsize = strlen(etagValue);
                                if (
                                    (etagsize < 2) ||
                                    (etagValue[0] != '"') ||
                                    (etagValue[etagsize - 1] != '"')
                                    )
                                {
                                    LogError("ETag is not a valid quoted string");
                                }
                                else
                                {
                                    const unsigned char* resp_content;
                                    size_t resp_len;
                                    /*Codes_SRS_TRANSPORTMULTITHTTP_17_089: [_DoWork shall assemble an IOTHUBMESSAGE_HANDLE from the received HTTP content (using the responseContent buffer).] */
                                    resp_content = BUFFER_u_char(responseContent);
                                    resp_len = BUFFER_length(responseContent);
                                    IOTHUB_MESSAGE_HANDLE receivedMessage = IoTHubMessage_CreateFromByteArray(resp_content, resp_len);
                                    if (receivedMessage == NULL)
                                    {
                                        /*Codes_SRS_TRANSPORTMULTITHTTP_17_092: [If assembling the message fails in any way, then _DoWork shall "abandon" the message.]*/
                                        LogError("unable to IoTHubMessage_CreateFromByteArray, trying to abandon the message... ");
                                        if (!abandonOrAcceptMessage(handleData, deviceData, etagValue, IOTHUBMESSAGE_ABANDONED))
                                        {
                                            LogError("HTTP Transport layer failed to report ABANDON disposition");
                                        }
                                    }
                                    else
                                    {
                                        if (retrieve_message_properties(responseHTTPHeaders, receivedMessage) != 0)
                                        {
                                            if (!abandonOrAcceptMessage(handleData, deviceData, etagValue, IOTHUBMESSAGE_ABANDONED))
                                            {
                                                LogError("HTTP Transport layer failed to report ABANDON disposition");
                                            }
                                        }
                                        else
                                        {
                                            MESSAGE_CALLBACK_INFO* messageData = MESSAGE_CALLBACK_INFO_Create(receivedMessage, handleData, deviceData, etagValue);
                                            if (messageData == NULL)
                                            {
                                                /*Codes_SRS_TRANSPORTMULTITHTTP_10_006: [If assembling the transport context fails, _DoWork shall "abandon" the message.] */
                                                LogError("failed to assemble callback info");
                                                if (!abandonOrAcceptMessage(handleData, deviceData, etagValue, IOTHUBMESSAGE_ABANDONED))
                                                {
                                                    LogError("HTTP Transport layer failed to report ABANDON disposition");
                                                }
                                            }
                                            else
                                            {
                                                bool abandon;
                                                if (handleData->transport_callbacks.msg_cb(messageData, deviceData->device_transport_ctx))
                                                {
                                                    abandon = false;
                                                }
                                                else
                                                {
                                                    LogError("IoTHubClientCore_LL_MessageCallback failed");
                                                    abandon = true;
                                                }

                                                /*Codes_SRS_TRANSPORTMULTITHTTP_17_096: [If IoTHubClientCore_LL_MessageCallback returns false then _DoWork shall "abandon" the message.] */
                                                if (abandon)
                                                {
                                                    (void)IoTHubTransportHttp_SendMessageDisposition(messageData, IOTHUBMESSAGE_ABANDONED);
                                                }
                                            }
                                        }
                                        IoTHubMessage_Destroy(receivedMessage);
                                    }
                                }
                            }
                        }
                    }
                    BUFFER_delete(responseContent);
                }
                HTTPHeaders_Free(responseHTTPHeaders);
            }
        }
        else
        {
            /*isPollingAllowed is false... */
            /*do nothing "shall be ignored*/
        }
    }
}

static IOTHUB_PROCESS_ITEM_RESULT IoTHubTransportHttp_ProcessItem(TRANSPORT_LL_HANDLE handle, IOTHUB_IDENTITY_TYPE item_type, IOTHUB_IDENTITY_INFO* iothub_item)
{
    (void)handle;
    (void)item_type;
    (void)iothub_item;
    LogError("Currently Not Supported.");
    return IOTHUB_PROCESS_ERROR;
}

static void IoTHubTransportHttp_DoWork(TRANSPORT_LL_HANDLE handle)
{
    /*Codes_SRS_TRANSPORTMULTITHTTP_17_049: [ If handle is NULL, then IoTHubTransportHttp_DoWork shall do nothing. ]*/
    if (handle != NULL)
    {
        HTTPTRANSPORT_HANDLE_DATA* handleData = (HTTPTRANSPORT_HANDLE_DATA*)handle;
        IOTHUB_DEVICE_HANDLE* listItem;
        size_t deviceListSize = VECTOR_size(handleData->perDeviceList);
        /*Codes_SRS_TRANSPORTMULTITHTTP_17_052: [ IoTHubTransportHttp_DoWork shall perform a round-robin loop through every deviceHandle in the transport device list. ]*/
        /*Codes_SRS_TRANSPORTMULTITHTTP_17_050: [ IoTHubTransportHttp_DoWork shall call loop through the device list. ] */
        /*Codes_SRS_TRANSPORTMULTITHTTP_17_051: [ IF the list is empty, then IoTHubTransportHttp_DoWork shall do nothing. ]*/
        for (size_t i = 0; i < deviceListSize; i++)
        {
            listItem = (IOTHUB_DEVICE_HANDLE *)VECTOR_element(handleData->perDeviceList, i);
            HTTPTRANSPORT_PERDEVICE_DATA* perDeviceItem = *(HTTPTRANSPORT_PERDEVICE_DATA**)(listItem);
            DoEvent(handleData, perDeviceItem);
            DoMessages(handleData, perDeviceItem);
        }
    }
    else
    {
        LogError("Invalid Argument NULL call on DoWork.");
    }
}

static IOTHUB_CLIENT_RESULT IoTHubTransportHttp_GetSendStatus(IOTHUB_DEVICE_HANDLE handle, IOTHUB_CLIENT_STATUS *iotHubClientStatus)
{
    IOTHUB_CLIENT_RESULT result;

    if (handle == NULL)
    {
        /*Codes_SRS_TRANSPORTMULTITHTTP_17_111: [ IoTHubTransportHttp_GetSendStatus shall return IOTHUB_CLIENT_INVALID_ARG if called with NULL parameter. ]*/
        result = IOTHUB_CLIENT_INVALID_ARG;
        LogError("Invalid handle to IoTHubClient HTTP transport instance.");
    }
    else if (iotHubClientStatus == NULL)
    {
        result = IOTHUB_CLIENT_INVALID_ARG;
        LogError("Invalid pointer to output parameter IOTHUB_CLIENT_STATUS.");
    }
    else
    {
        /*Codes_SRS_TRANSPORTMULTITHTTP_17_138: [ IoTHubTransportHttp_GetSendStatus shall locate deviceHandle in the transport device list by calling list_find_if. ]*/
        IOTHUB_DEVICE_HANDLE* listItem = get_perDeviceDataItem(handle);
        if (listItem == NULL)
        {
            /*Codes_SRS_TRANSPORTMULTITHTTP_17_139: [ If the device structure is not found, then this function shall fail and return with IOTHUB_CLIENT_INVALID_ARG. ]*/
            result = IOTHUB_CLIENT_INVALID_ARG;
            LogError("Device not found in transport list.");
        }
        else
        {
            HTTPTRANSPORT_PERDEVICE_DATA* deviceData = (HTTPTRANSPORT_PERDEVICE_DATA*)(*listItem);
            /* Codes_SRS_TRANSPORTMULTITHTTP_17_113: [ IoTHubTransportHttp_GetSendStatus shall return IOTHUB_CLIENT_OK and status IOTHUB_CLIENT_SEND_STATUS_BUSY if there are currently event items to be sent or being sent. ] */
            if (!DList_IsListEmpty(deviceData->waitingToSend))
            {
                *iotHubClientStatus = IOTHUB_CLIENT_SEND_STATUS_BUSY;
            }
            /* Codes_SRS_TRANSPORTMULTITHTTP_17_112: [ IoTHubTransportHttp_GetSendStatus shall return IOTHUB_CLIENT_OK and status IOTHUB_CLIENT_SEND_STATUS_IDLE if there are currently no event items to be sent or being sent. ] */
            else
            {
                *iotHubClientStatus = IOTHUB_CLIENT_SEND_STATUS_IDLE;
            }
            result = IOTHUB_CLIENT_OK;
        }
    }

    return result;
}

static IOTHUB_CLIENT_RESULT IoTHubTransportHttp_SetOption(TRANSPORT_LL_HANDLE handle, const char* option, const void* value)
{
    IOTHUB_CLIENT_RESULT result;
    /*Codes_SRS_TRANSPORTMULTITHTTP_17_114: [If handle parameter is NULL then IoTHubTransportHttp_SetOption shall return IOTHUB_CLIENT_INVALID_ARG.] */
    /*Codes_SRS_TRANSPORTMULTITHTTP_17_115: [If option parameter is NULL then IoTHubTransportHttp_SetOption shall return IOTHUB_CLIENT_INVALID_ARG.] */
    /*Codes_SRS_TRANSPORTMULTITHTTP_17_116: [If value parameter is NULL then IoTHubTransportHttp_SetOption shall return IOTHUB_CLIENT_INVALID_ARG.] */
    if (
        (handle == NULL) ||
        (option == NULL) ||
        (value == NULL)
        )
    {
        result = IOTHUB_CLIENT_INVALID_ARG;
        LogError("invalid parameter (NULL) passed to IoTHubTransportHttp_SetOption");
    }
    else
    {
        HTTPTRANSPORT_HANDLE_DATA* handleData = (HTTPTRANSPORT_HANDLE_DATA*)handle;
        /*Codes_SRS_TRANSPORTMULTITHTTP_17_120: ["Batching"] */
        if (strcmp(OPTION_BATCHING, option) == 0)
        {
            /*Codes_SRS_TRANSPORTMULTITHTTP_17_117: [If optionName is an option handled by IoTHubTransportHttp then it shall be set.] */
            handleData->doBatchedTransfers = *(bool*)value;
            result = IOTHUB_CLIENT_OK;
        }
        /*Codes_SRS_TRANSPORTMULTITHTTP_17_121: ["MinimumPollingTime"] */
        else if (strcmp(OPTION_MIN_POLLING_TIME, option) == 0)
        {
            handleData->getMinimumPollingTime = *(unsigned int*)value;
            result = IOTHUB_CLIENT_OK;
        }
        else
        {
            /*Codes_SRS_TRANSPORTMULTITHTTP_17_126: [ "TrustedCerts"] */
            /*Codes_SRS_TRANSPORTMULTITHTTP_17_127: [ NULL shall be allowed. ]*/
            /*Codes_SRS_TRANSPORTMULTITHTTP_17_129: [ This option shall passed down to the lower layer by calling HTTPAPIEX_SetOption. ]*/
            /*Codes_SRS_TRANSPORTMULTITHTTP_17_118: [Otherwise, IoTHubTransport_Http shall call HTTPAPIEX_SetOption with the same parameters and return the translated code.] */
            HTTPAPIEX_RESULT HTTPAPIEX_result = HTTPAPIEX_SetOption(handleData->httpApiExHandle, option, value);
            /*Codes_SRS_TRANSPORTMULTITHTTP_17_119: [The following table translates HTTPAPIEX return codes to IOTHUB_CLIENT_RESULT return codes:] */
            if (HTTPAPIEX_result == HTTPAPIEX_OK)
            {
                result = IOTHUB_CLIENT_OK;
            }
            else if (HTTPAPIEX_result == HTTPAPIEX_INVALID_ARG)
            {
                result = IOTHUB_CLIENT_INVALID_ARG;
                LogError("HTTPAPIEX_SetOption failed");
            }
            else
            {
                result = IOTHUB_CLIENT_ERROR;
                LogError("HTTPAPIEX_SetOption failed");
            }
        }
    }
    return result;
}

static STRING_HANDLE IoTHubTransportHttp_GetHostname(TRANSPORT_LL_HANDLE handle)
{
    STRING_HANDLE result;
    /*Codes_SRS_TRANSPORTMULTITHTTP_02_001: [ If handle is NULL then IoTHubTransportHttp_GetHostname shall fail and return NULL. ]*/
    if (handle == NULL)
    {
        LogError("invalid parameter handle=%p", handle);
        result = NULL;
    }
    /*Codes_SRS_TRANSPORTMULTITHTTP_02_002: [ Otherwise IoTHubTransportHttp_GetHostname shall return a non-NULL STRING_HANDLE containing the hostname. ]*/
    else if ((result = STRING_clone(((HTTPTRANSPORT_HANDLE_DATA*)(handle))->hostName)) == NULL)
    {
        LogError("Cannot provide the target host name (STRING_clone failed).");
    }

    return result;
}

static int IoTHubTransportHttp_SetRetryPolicy(TRANSPORT_LL_HANDLE handle, IOTHUB_CLIENT_RETRY_POLICY retryPolicy, size_t retryTimeoutLimitInSeconds)
{
    int result;
    (void)handle;
    (void)retryPolicy;
    (void)retryTimeoutLimitInSeconds;

    /* Retry Policy is not currently not available for HTTP */

    result = 0;
    return result;
}

static int IotHubTransportHttp_Subscribe_InputQueue(IOTHUB_DEVICE_HANDLE handle)
{
    (void)handle;
    LogError("HTTP does not support input queues");
    return MU_FAILURE;
}

static void IotHubTransportHttp_Unsubscribe_InputQueue(IOTHUB_DEVICE_HANDLE handle)
{
    (void)handle;
    LogError("HTTP does not support input queues");
}

int IoTHubTransportHttp_SetCallbackContext(TRANSPORT_LL_HANDLE handle, void* ctx)
{
    int result;
    if (handle == NULL)
    {
        LogError("Invalid parameter specified handle: %p", handle);
        result = MU_FAILURE;
    }
    else
    {
        HTTPTRANSPORT_HANDLE_DATA* handleData = (HTTPTRANSPORT_HANDLE_DATA*)handle;
        handleData->transport_ctx = ctx;
        result = 0;
    }
    return result;
}

static int IoTHubTransportHttp_GetSupportedPlatformInfo(TRANSPORT_LL_HANDLE handle, PLATFORM_INFO_OPTION* info)
{
    int result;
    if (handle == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        *info = PLATFORM_INFO_OPTION_DEFAULT;
        result = 0;
    }

    return result;
}

/*Codes_SRS_TRANSPORTMULTITHTTP_17_125: [This function shall return a pointer to a structure of type TRANSPORT_PROVIDER having the following values for its fields:] */
static TRANSPORT_PROVIDER thisTransportProvider =
{
    IoTHubTransportHttp_SendMessageDisposition,     /*pfIotHubTransport_SendMessageDisposition IoTHubTransport_SendMessageDisposition;*/
    IoTHubTransportHttp_Subscribe_DeviceMethod,     /*pfIoTHubTransport_Subscribe_DeviceMethod IoTHubTransport_Subscribe_DeviceMethod;*/
    IoTHubTransportHttp_Unsubscribe_DeviceMethod,   /*pfIoTHubTransport_Unsubscribe_DeviceMethod IoTHubTransport_Unsubscribe_DeviceMethod;*/
    IoTHubTransportHttp_DeviceMethod_Response,      /*pfIoTHubTransport_DeviceMethod_Response IoTHubTransport_DeviceMethod_Response;*/
    IoTHubTransportHttp_Subscribe_DeviceTwin,       /*pfIoTHubTransport_Subscribe_DeviceTwin IoTHubTransport_Subscribe_DeviceTwin;*/
    IoTHubTransportHttp_Unsubscribe_DeviceTwin,     /*pfIoTHubTransport_Unsubscribe_DeviceTwin IoTHubTransport_Unsubscribe_DeviceTwin;*/
    IoTHubTransportHttp_ProcessItem,                /*pfIoTHubTransport_ProcessItem IoTHubTransport_ProcessItem;*/
    IoTHubTransportHttp_GetHostname,                /*pfIoTHubTransport_GetHostname IoTHubTransport_GetHostname;*/
    IoTHubTransportHttp_SetOption,                  /*pfIoTHubTransport_SetOption IoTHubTransport_SetOption;*/
    IoTHubTransportHttp_Create,                     /*pfIoTHubTransport_Create IoTHubTransport_Create;*/
    IoTHubTransportHttp_Destroy,                    /*pfIoTHubTransport_Destroy IoTHubTransport_Destroy;*/
    IoTHubTransportHttp_Register,                   /*pfIotHubTransport_Register IoTHubTransport_Register;*/
    IoTHubTransportHttp_Unregister,                 /*pfIotHubTransport_Unregister IoTHubTransport_Unegister;*/
    IoTHubTransportHttp_Subscribe,                  /*pfIoTHubTransport_Subscribe IoTHubTransport_Subscribe;*/
    IoTHubTransportHttp_Unsubscribe,                /*pfIoTHubTransport_Unsubscribe IoTHubTransport_Unsubscribe;*/
    IoTHubTransportHttp_DoWork,                     /*pfIoTHubTransport_DoWork IoTHubTransport_DoWork;*/
    IoTHubTransportHttp_SetRetryPolicy,             /*pfIoTHubTransport_DoWork IoTHubTransport_SetRetryPolicy;*/
    IoTHubTransportHttp_GetSendStatus,              /*pfIoTHubTransport_GetSendStatus IoTHubTransport_GetSendStatus;*/
    IotHubTransportHttp_Subscribe_InputQueue,       /*pfIoTHubTransport_Subscribe_InputQueue IoTHubTransport_Subscribe_InputQueue; */
    IotHubTransportHttp_Unsubscribe_InputQueue,     /*pfIoTHubTransport_Unsubscribe_InputQueue IoTHubTransport_Unsubscribe_InputQueue; */
    IoTHubTransportHttp_SetCallbackContext,         /*pfIoTHubTransport_SetTransportCallbacks IoTHubTransport_SetTransportCallbacks; */
    IoTHubTransportHttp_GetTwinAsync,               /*pfIoTHubTransport_GetTwinAsync IoTHubTransport_GetTwinAsync;*/
    IoTHubTransportHttp_GetSupportedPlatformInfo      /*pfIoTHubTransport_GetSupportedPlatformInfo IoTHubTransport_GetSupportedPlatformInfo;*/
};

const TRANSPORT_PROVIDER* HTTP_Protocol(void)
{
    return &thisTransportProvider;
}
