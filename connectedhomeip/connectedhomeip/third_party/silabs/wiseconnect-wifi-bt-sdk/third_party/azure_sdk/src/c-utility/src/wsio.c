 // Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include "azure_c_shared_utility/gballoc.h"
#include "azure_c_shared_utility/wsio.h"
#include "azure_c_shared_utility/xlogging.h"
#include "azure_c_shared_utility/singlylinkedlist.h"
#include "azure_c_shared_utility/optionhandler.h"
#include "azure_c_shared_utility/xio.h"
#include "azure_c_shared_utility/shared_util_options.h"
#include "azure_c_shared_utility/crt_abstractions.h"
#include "azure_c_shared_utility/uws_client.h"
#include "azure_c_shared_utility/optimize_size.h"

static const char* WSIO_OPTIONS = "WSIOOptions";

typedef enum IO_STATE_TAG
{
    IO_STATE_NOT_OPEN,
    IO_STATE_OPENING,
    IO_STATE_OPEN,
    IO_STATE_CLOSING,
    IO_STATE_ERROR
} IO_STATE;

typedef struct PENDING_IO_TAG
{
    ON_SEND_COMPLETE on_send_complete;
    void* callback_context;
    void* wsio;
} PENDING_IO;

typedef struct WSIO_INSTANCE_TAG
{
    ON_BYTES_RECEIVED on_bytes_received;
    void* on_bytes_received_context;
    ON_IO_OPEN_COMPLETE on_io_open_complete;
    void* on_io_open_complete_context;
    ON_IO_ERROR on_io_error;
    void* on_io_error_context;
    ON_IO_CLOSE_COMPLETE on_io_close_complete;
    void* on_io_close_complete_context;
    IO_STATE io_state;
    SINGLYLINKEDLIST_HANDLE pending_io_list;
    UWS_CLIENT_HANDLE uws;
} WSIO_INSTANCE;

static void indicate_error(WSIO_INSTANCE* wsio_instance)
{
    wsio_instance->io_state = IO_STATE_ERROR;
    wsio_instance->on_io_error(wsio_instance->on_io_error_context);
}

static void indicate_open_complete(WSIO_INSTANCE* ws_io_instance, IO_OPEN_RESULT open_result)
{
    ws_io_instance->on_io_open_complete(ws_io_instance->on_io_open_complete_context, open_result);
}

static void complete_send_item(LIST_ITEM_HANDLE pending_io_list_item, IO_SEND_RESULT io_send_result)
{
    PENDING_IO* pending_io = (PENDING_IO*)singlylinkedlist_item_get_value(pending_io_list_item);
    if (pending_io != NULL)
    {
        WSIO_INSTANCE* wsio_instance = (WSIO_INSTANCE*)pending_io->wsio;

        /* Codes_SRS_WSIO_01_145: [ Removing it from the list shall be done by calling singlylinkedlist_remove. ]*/
        if (wsio_instance == NULL ||
            singlylinkedlist_remove(wsio_instance->pending_io_list, pending_io_list_item) != 0)
        {
            LogError("Failed removing pending IO from linked list.");
        }

        /* Codes_SRS_WSIO_01_105: [ The argument on_send_complete shall be optional, if NULL is passed by the caller then no send complete callback shall be triggered. ]*/
        if (pending_io->on_send_complete != NULL)
        {
            pending_io->on_send_complete(pending_io->callback_context, io_send_result);
        }

        /* Codes_SRS_WSIO_01_144: [ Also the pending IO data shall be freed. ]*/
        free(pending_io);
    }
}

static void on_underlying_ws_send_frame_complete(void* context, WS_SEND_FRAME_RESULT ws_send_frame_result)
{
    if (context == NULL)
    {
        LogError("NULL context for on_underlying_ws_send_frame_complete");
    }
    else
    {
        IO_SEND_RESULT io_send_result;
        LIST_ITEM_HANDLE list_item_handle = (LIST_ITEM_HANDLE)context;

        /* Codes_SRS_WSIO_01_143: [ When on_underlying_ws_send_frame_complete is called after sending a WebSocket frame, the pending IO shall be removed from the list. ]*/
        switch (ws_send_frame_result)
        {
        default:
            /* Codes_SRS_WSIO_01_148: [ When on_underlying_ws_send_frame_complete is called with any other error code, the callback on_send_complete shall be called with IO_SEND_ERROR. ]*/
            LogError("Frame send error with result %d", (int)ws_send_frame_result);
            io_send_result = IO_SEND_ERROR;
            break;

        case WS_SEND_FRAME_OK:
            /* Codes_SRS_WSIO_01_146: [ When on_underlying_ws_send_frame_complete is called with WS_SEND_OK, the callback on_send_complete shall be called with IO_SEND_OK. ]*/
            io_send_result = IO_SEND_OK;
            break;

        case WS_SEND_FRAME_CANCELLED:
            /* Codes_SRS_WSIO_01_147: [ When on_underlying_ws_send_frame_complete is called with WS_SEND_CANCELLED, the callback on_send_complete shall be called with IO_SEND_CANCELLED. ]*/
            io_send_result = IO_SEND_CANCELLED;
            break;
        }

        complete_send_item(list_item_handle, io_send_result);
    }
}

static void on_underlying_ws_close_complete(void* context)
{
    WSIO_INSTANCE* wsio_instance = (WSIO_INSTANCE*)context;
    if (wsio_instance == NULL)
    {
        /* Codes_SRS_WSIO_01_161: [ If the context passed to on_underlying_ws_close_complete is NULL, on_underlying_ws_close_complete shall do nothing. ]*/
        LogError("NULL context passed to on_underlying_ws_close_complete");
    }
    else
    {
        wsio_instance->io_state = IO_STATE_NOT_OPEN;

        /* Codes_SRS_WSIO_01_160: [ If NULL was passed to wsio_close no callback shall be called. ]*/
        if (wsio_instance->on_io_close_complete != NULL)
        {
            /* Codes_SRS_WSIO_01_159: [ When on_underlying_ws_close_complete while the IO is closing (after wsio_close), the close shall be indicated up by calling the on_io_close_complete callback passed to wsio_close. ]*/
            /* Codes_SRS_WSIO_01_163: [ When on_io_close_complete is called, the context passed to wsio_close shall be passed as argument to on_io_close_complete. ]*/
            wsio_instance->on_io_close_complete(wsio_instance->on_io_close_complete_context);
        }
    }
}

static int internal_close(WSIO_INSTANCE* wsio_instance, ON_IO_CLOSE_COMPLETE on_io_close_complete, void* on_io_close_complete_context)
{
    int result;

    /* Codes_SRS_WSIO_01_089: [ wsio_close after a wsio_close shall fail and return a non-zero value.  ]*/
    /* Codes_SRS_WSIO_01_088: [ wsio_close when no open action has been issued shall fail and return a non-zero value. ]*/
    if (wsio_instance->io_state == IO_STATE_NOT_OPEN)
    {
        LogError("wsio_close when not open.");
        result = MU_FAILURE;
    }
    else
    {
        if (wsio_instance->io_state == IO_STATE_OPENING)
        {
            wsio_instance->io_state = IO_STATE_NOT_OPEN;
            indicate_open_complete(wsio_instance, IO_OPEN_CANCELLED);
            result = 0;
        }
        else if (wsio_instance->io_state == IO_STATE_CLOSING)
        {
            LogError("Already closing");
            result = MU_FAILURE;
        }
        else
        {
            LIST_ITEM_HANDLE first_pending_io;

            wsio_instance->io_state = IO_STATE_CLOSING;

            wsio_instance->on_io_close_complete = on_io_close_complete;
            wsio_instance->on_io_close_complete_context = on_io_close_complete_context;

            /* Codes_SRS_WSIO_01_087: [ wsio_close shall call uws_client_close_async while passing as argument the IO handle created in wsio_create.  ]*/
            if (uws_client_close_async(wsio_instance->uws, on_underlying_ws_close_complete, wsio_instance) != 0)
            {
                /* Codes_SRS_WSIO_01_164: [ When uws_client_close_async fails, wsio_close shall call the on_io_close_complete callback and continue. ] */
                if (wsio_instance->on_io_close_complete != NULL)
                {
                    wsio_instance->on_io_close_complete(wsio_instance->on_io_close_complete_context);
                }
            }

            /* Codes_SRS_WSIO_01_085: [ wsio_close shall close the websockets IO if an open action is either pending or has completed successfully (if the IO is open).  ]*/
            /* Codes_SRS_WSIO_01_091: [ wsio_close shall obtain all the pending IO items by repetitively querying for the head of the pending IO list and freeing that head item. ]*/
            /* Codes_SRS_WSIO_01_092: [ Obtaining the head of the pending IO list shall be done by calling singlylinkedlist_get_head_item. ]*/
            while ((first_pending_io = singlylinkedlist_get_head_item(wsio_instance->pending_io_list)) != NULL)
            {
                complete_send_item(first_pending_io, IO_SEND_CANCELLED);
            }

            /* Codes_SRS_WSIO_01_133: [ On success wsio_close shall return 0. ]*/
            result = 0;
            wsio_instance->io_state = IO_STATE_NOT_OPEN;
        }
    }
    return result;
}

CONCRETE_IO_HANDLE wsio_create(void* io_create_parameters)
{
    /* Codes_SRS_WSIO_01_066: [ io_create_parameters shall be used as a WSIO_CONFIG* . ]*/
    WSIO_CONFIG* ws_io_config = (WSIO_CONFIG*)io_create_parameters;
    WSIO_INSTANCE* result;

    /* Codes_SRS_WSIO_01_065: [ If the argument io_create_parameters is NULL then wsio_create shall return NULL. ]*/
    if ((ws_io_config == NULL) ||
        /* Codes_SRS_WSIO_01_067: [ If any of the members hostname, resource_name or protocol is NULL in WSIO_CONFIG then wsio_create shall return NULL. ]*/
        (ws_io_config->hostname == NULL) ||
        (ws_io_config->resource_name == NULL) ||
        (ws_io_config->protocol == NULL))
    {
        LogError("NULL io_create_parameters.");
        result = NULL;
    }
    else
    {
        /* Codes_SRS_WSIO_01_001: [wsio_create shall create an instance of wsio and return a non-NULL handle to it.] */
        result = (WSIO_INSTANCE*)calloc(1, sizeof(WSIO_INSTANCE));
        if (result == NULL)
        {
            /* Codes_SRS_WSIO_01_068: [ If allocating memory for the new wsio instance fails then wsio_create shall return NULL. ]*/
            LogError("Cannot allocate memory for the new WSIO instance.");
        }
        else
        {
            WS_PROTOCOL protocols;

            protocols.protocol = ws_io_config->protocol;

            result->on_bytes_received = NULL;
            result->on_bytes_received_context = NULL;
            result->on_io_open_complete = NULL;
            result->on_io_open_complete_context = NULL;
            result->on_io_error = NULL;
            result->on_io_error_context = NULL;
            result->on_io_close_complete = NULL;
            result->on_io_close_complete_context = NULL;

            /* Codes_SRS_WSIO_01_070: [ The underlying uws instance shall be created by calling uws_client_create_with_io. ]*/
            /* Codes_SRS_WSIO_01_071: [ The arguments for uws_client_create_with_io shall be: ]*/
            /* Codes_SRS_WSIO_01_185: [ - underlying_io_interface shall be set to the underlying_io_interface field in the io_create_parameters passed to wsio_create. ]*/
            /* Codes_SRS_WSIO_01_186: [ - underlying_io_parameters shall be set to the underlying_io_parameters field in the io_create_parameters passed to wsio_create. ]*/
            /* Codes_SRS_WSIO_01_072: [ - hostname set to the hostname field in the io_create_parameters passed to wsio_create. ]*/
            /* Codes_SRS_WSIO_01_130: [ - port set to the port field in the io_create_parameters passed to wsio_create. ]*/
            /* Codes_SRS_WSIO_01_128: [ - resource_name set to the resource_name field in the io_create_parameters passed to wsio_create. ]*/
            /* Codes_SRS_WSIO_01_129: [ - protocols shall be filled with only one structure, that shall have the protocol set to the value of the protocol field in the io_create_parameters passed to wsio_create. ]*/
            result->uws = uws_client_create_with_io(ws_io_config->underlying_io_interface, ws_io_config->underlying_io_parameters, ws_io_config->hostname, ws_io_config->port, ws_io_config->resource_name, &protocols, 1);
            if (result->uws == NULL)
            {
                /* Codes_SRS_WSIO_01_075: [ If uws_client_create_with_io fails, then wsio_create shall fail and return NULL. ]*/
                LogError("Cannot create uws instance.");
                free(result);
                result = NULL;
            }
            else
            {
                /* Codes_SRS_WSIO_01_076: [ wsio_create shall create a pending send IO list that is to be used to queue send packets by calling singlylinkedlist_create. ]*/
                result->pending_io_list = singlylinkedlist_create();
                if (result->pending_io_list == NULL)
                {
                    /* Codes_SRS_WSIO_01_077: [ If singlylinkedlist_create fails then wsio_create shall fail and return NULL. ]*/
                    LogError("Cannot create singly linked list.");
                    uws_client_destroy(result->uws);
                    free(result);
                    result = NULL;
                }
                else
                {
                    result->io_state = IO_STATE_NOT_OPEN;
                }
            }
        }
    }

    return result;
}

void wsio_destroy(CONCRETE_IO_HANDLE ws_io)
{
    /* Codes_SRS_WSIO_01_079: [ If ws_io is NULL, wsio_destroy shall do nothing.  ]*/
    if (ws_io == NULL)
    {
        LogError("NULL handle");
    }
    else
    {
        WSIO_INSTANCE* wsio_instance = (WSIO_INSTANCE*)ws_io;

        if (wsio_instance->io_state != IO_STATE_NOT_OPEN)
        {
            internal_close(wsio_instance, NULL, NULL);
        }

        /* Codes_SRS_WSIO_01_078: [ wsio_destroy shall free all resources associated with the wsio instance. ]*/
        /* Codes_SRS_WSIO_01_080: [ wsio_destroy shall destroy the uws instance created in wsio_create by calling uws_client_destroy. ]*/
        uws_client_destroy(wsio_instance->uws);
        /* Codes_SRS_WSIO_01_081: [ wsio_destroy shall free the list used to track the pending send IOs by calling singlylinkedlist_destroy. ]*/
        singlylinkedlist_destroy(wsio_instance->pending_io_list);
        free(ws_io);
    }
}

static void on_underlying_ws_open_complete(void* context, WS_OPEN_RESULT open_result)
{
    if (context == NULL)
    {
        /* Codes_SRS_WSIO_01_138: [ When on_underlying_ws_open_complete is called with a NULL context, it shall do nothing. ]*/
        LogError("NULL context in on_underlying_ws_open_complete");
    }
    else
    {
        WSIO_INSTANCE* wsio_instance = (WSIO_INSTANCE*)context;

        switch (wsio_instance->io_state)
        {
        default:
        /* Codes_SRS_WSIO_01_142: [ When on_underlying_ws_open_complete is called while in the CLOSING state an error shall be indicated by calling the on_io_error callback passed to wsio_open. ]*/
        case IO_STATE_CLOSING:
        /* Codes_SRS_WSIO_01_141: [ When on_underlying_ws_open_complete is called while in the ERROR state it shall indicate an error by calling the on_io_error callback passed to wsio_open. ]*/
        case IO_STATE_ERROR:
        /* Codes_SRS_WSIO_01_139: [ When on_underlying_ws_open_complete is called while in OPEN state it shall indicate an error by calling the on_io_error callback passed to wsio_open and switch to the ERROR state. ]*/
        case IO_STATE_OPEN:
            /* Codes_SRS_WSIO_01_140: [ When calling on_io_error, the on_io_error_context argument given in wsio_open shall be passed to the callback on_io_error. ]*/
            indicate_error(wsio_instance);
            break;

        case IO_STATE_OPENING:
            wsio_instance->io_state = IO_STATE_OPEN;

            switch (open_result)
            {
            default:
                /* Codes_SRS_WSIO_01_137: [ When on_underlying_ws_open_complete is called with any other error code while the IO is opening, the callback on_io_open_complete shall be called with IO_OPEN_ERROR. ]*/
                wsio_instance->io_state = IO_STATE_NOT_OPEN;
                indicate_open_complete(wsio_instance, IO_OPEN_ERROR);
                break;

            case WS_OPEN_CANCELLED:
                /* Codes_SRS_WSIO_01_149: [ When on_underlying_ws_open_complete is called with WS_OPEN_CANCELLED while the IO is opening, the callback on_io_open_complete shall be called with IO_OPEN_CANCELLED. ]*/
                wsio_instance->io_state = IO_STATE_NOT_OPEN;
                indicate_open_complete(wsio_instance, IO_OPEN_CANCELLED);
                break;

            case WS_OPEN_OK:
                /* Codes_SRS_WSIO_01_136: [ When on_underlying_ws_open_complete is called with WS_OPEN_OK while the IO is opening, the callback on_io_open_complete shall be called with IO_OPEN_OK. ]*/
                wsio_instance->io_state = IO_STATE_OPEN;
                indicate_open_complete(wsio_instance, IO_OPEN_OK);
                break;
            }

            break;
        }
    }
}

static void on_underlying_ws_frame_received(void* context, unsigned char frame_type, const unsigned char* buffer, size_t size)
{
    if (context == NULL)
    {
        /* Codes_SRS_WSIO_01_150: [ If on_underlying_ws_frame_received is called with NULL context it shall do nothing. ]*/
        LogError("NULL context for on_underlying_ws_frame_received");
    }
    else
    {
        WSIO_INSTANCE* wsio_instance = (WSIO_INSTANCE*)context;

        if (wsio_instance->io_state != IO_STATE_OPEN)
        {
            /* Codes_SRS_WSIO_01_126: [ If on_underlying_ws_frame_received is called while the IO is in any state other than OPEN, it shall do nothing. ]*/
            LogError("on_underlying_ws_frame_received called in a bad state.");
        }
        else
        {
            if (frame_type != WS_FRAME_TYPE_BINARY)
            {
                /* Codes_SRS_WSIO_01_151: [ If the WebSocket frame type is not binary then an error shall be indicated by calling the on_io_error callback passed to wsio_open. ]*/
                LogError("Invalid non binary WebSocket frame received.");
                /* Codes_SRS_WSIO_01_152: [ When calling on_io_error, the on_io_error_context argument given in wsio_open shall be passed to the callback on_io_error. ]*/
                indicate_error(wsio_instance);
            }
            else
            {
                /* Codes_SRS_WSIO_01_153: [ When on_underlying_ws_frame_received is called with zero size, no bytes shall be indicated up as received. ]*/
                if (size > 0)
                {
                    if (buffer == NULL)
                    {
                        /* Codes_SRS_WSIO_01_154: [ When on_underlying_ws_frame_received is called with a positive size and a NULL buffer, an error shall be indicated by calling the on_io_error callback passed to wsio_open. ]*/
                        LogError("NULL buffer received for Websocket frame with positive payload length.");
                        indicate_error(wsio_instance);
                    }
                    else
                    {
                        /* Codes_SRS_WSIO_01_124: [ When on_underlying_ws_frame_received is called the bytes in the frame shall be indicated by calling the on_bytes_received callback passed to wsio_open. ]*/
                        /* Codes_SRS_WSIO_01_125: [ When calling on_bytes_received, the on_bytes_received_context argument given in wsio_open shall be passed to the callback on_bytes_received. ]*/
                        wsio_instance->on_bytes_received(wsio_instance->on_bytes_received_context, buffer, size);
                    }
                }
            }
        }
    }
}

static void on_underlying_ws_peer_closed(void* context, uint16_t* close_code, const unsigned char* extra_data, size_t extra_data_length)
{
    /* Codes_SRS_WSIO_01_168: [ The close_code, extra_data and extra_data_length arguments shall be ignored. ]*/
    (void)close_code;
    (void)extra_data;
    (void)extra_data_length;

    if (context == NULL)
    {
        /* Codes_SRS_WSIO_01_167: [ If on_underlying_ws_peer_closed is called with a NULL context it shall do nothing. ]*/
        LogError("NULL context for on_underlying_ws_peer_closed");
    }
    else
    {
        WSIO_INSTANCE* wsio_instance = (WSIO_INSTANCE*)context;

        switch (wsio_instance->io_state)
        {
        default:
            /* Codes_SRS_WSIO_01_166: [ When on_underlying_ws_peer_closed and the state of the IO is OPEN an error shall be indicated by calling the on_io_error callback passed to wsio_open. ]*/
        case IO_STATE_OPEN:
            /* Codes_SRS_WSIO_01_169: [ When on_underlying_ws_peer_closed and the state of the IO is CLOSING an error shall be indicated by calling the on_io_error callback passed to wsio_open. ]*/
        case IO_STATE_CLOSING:
            indicate_error(wsio_instance);
            break;

        case IO_STATE_NOT_OPEN:
            // Codes_SRS_WSIO_07_001: [When on_underlying_ws_peer_closed and the state of the IO is NOT_OPEN an error will be raised and the io_state will remain as NOT_OPEN]
            indicate_error(wsio_instance);
            wsio_instance->io_state = IO_STATE_NOT_OPEN;
            break;

        case IO_STATE_OPENING:
            /* Codes_SRS_WSIO_01_170: [ When on_underlying_ws_peer_closed and the state of the IO is OPENING an error shall be indicated by calling the on_io_open_complete callback passed to wsio_open with the error code IO_OPEN_ERROR. ]*/
            wsio_instance->io_state = IO_STATE_NOT_OPEN;
            indicate_open_complete(wsio_instance, IO_OPEN_ERROR);
            break;
        }
    }
}

static void on_underlying_ws_error(void* context, WS_ERROR ws_error)
{
    (void)ws_error;
    /* Don't have much to do with the error here */
    LogError("on_underlying_ws_error called with error code %d", (int)ws_error);

    if (context == NULL)
    {
        /* Codes_SRS_WSIO_01_135: [ When on_underlying_ws_error is called with a NULL context, it shall do nothing. ]*/
        LogError("NULL context in on_underlying_ws_error");
    }
    else
    {
        WSIO_INSTANCE* wsio_instance = (WSIO_INSTANCE*)context;

        if (wsio_instance->io_state == IO_STATE_OPENING)
        {
            /* Codes_SRS_WSIO_01_123: [ When calling on_io_error, the on_io_error_context argument given in wsio_open shall be passed to the callback on_io_error. ]*/
            wsio_instance->on_io_open_complete(wsio_instance->on_io_open_complete_context, IO_OPEN_ERROR);
            wsio_instance->io_state = IO_STATE_NOT_OPEN;
        }
        else
        {
            /* Codes_SRS_WSIO_01_123: [ When calling on_io_error, the on_io_error_context argument given in wsio_open shall be passed to the callback on_io_error. ]*/
            wsio_instance->on_io_error(wsio_instance->on_io_error_context);
        }
    }
}

int wsio_open(CONCRETE_IO_HANDLE ws_io, ON_IO_OPEN_COMPLETE on_io_open_complete, void* on_io_open_complete_context, ON_BYTES_RECEIVED on_bytes_received, void* on_bytes_received_context, ON_IO_ERROR on_io_error, void* on_io_error_context)
{
    int result = 0;

    if ((ws_io == NULL) ||
        (on_io_open_complete == NULL) ||
        (on_bytes_received == NULL) ||
        (on_io_error == NULL))
    {
        /* Codes_SRS_WSIO_01_132: [ If any of the arguments ws_io, on_io_open_complete, on_bytes_received, on_io_error is NULL, wsio_open shall fail and return a non-zero value. ]*/
        LogError("Bad arguments: ws_io=%p, on_io_open_complete=%p, on_bytes_received=%p, on_io_error=%p",
            ws_io, on_io_open_complete, on_bytes_received, on_io_error);
        result = MU_FAILURE;
    }
    else
    {
        WSIO_INSTANCE* wsio_instance = (WSIO_INSTANCE*)ws_io;

        if (wsio_instance->io_state != IO_STATE_NOT_OPEN)
        {
            /* Codes_SRS_WSIO_01_165: [ wsio_open when CLOSING shall fail and return a non-zero value. ]*/
            /* Codes_SRS_WSIO_01_131: [ wsio_open when already OPEN or OPENING shall fail and return a non-zero value. ]*/
            LogError("wsio has already been opened current state: %d", wsio_instance->io_state);
            result = MU_FAILURE;
        }
        else
        {
            wsio_instance->on_bytes_received = on_bytes_received;
            wsio_instance->on_bytes_received_context = on_bytes_received_context;
            wsio_instance->on_io_open_complete = on_io_open_complete;
            wsio_instance->on_io_open_complete_context = on_io_open_complete_context;
            wsio_instance->on_io_error = on_io_error;
            wsio_instance->on_io_error_context = on_io_error_context;

            wsio_instance->io_state = IO_STATE_OPENING;

            /* Codes_SRS_WSIO_01_082: [ wsio_open shall open the underlying uws instance by calling uws_client_open_async and providing the uws handle created in wsio_create as argument. ] */
            if (uws_client_open_async(wsio_instance->uws, on_underlying_ws_open_complete, wsio_instance, on_underlying_ws_frame_received, wsio_instance, on_underlying_ws_peer_closed, wsio_instance, on_underlying_ws_error, wsio_instance) != 0)
            {
                /* Codes_SRS_WSIO_01_084: [ If opening the underlying uws instance fails then wsio_open shall fail and return a non-zero value. ]*/
                LogError("Opening the uws instance failed.");
                wsio_instance->io_state = IO_STATE_NOT_OPEN;
                result = MU_FAILURE;
            }
            else
            {
                /* Codes_SRS_WSIO_01_083: [ On success, wsio_open shall return 0. ]*/
                result = 0;
            }
        }
    }

    return result;
}

int wsio_close(CONCRETE_IO_HANDLE ws_io, ON_IO_CLOSE_COMPLETE on_io_close_complete, void* on_io_close_complete_context)
{
    int result = 0;

    if (ws_io == NULL)
    {
        /* Codes_SRS_WSIO_01_086: [ if ws_io is NULL, wsio_close shall return a non-zero value.  ]*/
        LogError("NULL handle");
        result = MU_FAILURE;
    }
    else
    {
        WSIO_INSTANCE* wsio_instance = (WSIO_INSTANCE*)ws_io;

        if (internal_close(wsio_instance, on_io_close_complete, on_io_close_complete_context) != 0)
        {
            result = MU_FAILURE;
        }
        else
        {
            result = 0;
        }
    }

    return result;
}

int wsio_send(CONCRETE_IO_HANDLE ws_io, const void* buffer, size_t size, ON_SEND_COMPLETE on_send_complete, void* callback_context)
{
    int result;

    /* Codes_SRS_WSIO_01_100: [ If any of the arguments ws_io or buffer are NULL, wsio_send shall fail and return a non-zero value. ]*/
    if ((ws_io == NULL) ||
        (buffer == NULL) ||
        /* Codes_SRS_WSIO_01_101: [ If size is zero then wsio_send shall fail and return a non-zero value. ]*/
        (size == 0))
    {
        LogError("Bad arguments: ws_io=%p, buffer=%p, size=%u",
            ws_io, buffer, (unsigned int)size);
        result = MU_FAILURE;
    }
    else
    {
        WSIO_INSTANCE* wsio_instance = (WSIO_INSTANCE*)ws_io;

        if (wsio_instance->io_state != IO_STATE_OPEN)
        {
            /* Codes_SRS_WSIO_01_099: [ If the wsio is not OPEN (open has not been called or is still in progress) then wsio_send shall fail and return a non-zero value. ]*/
            LogError("Attempting to send when not open");
            result = MU_FAILURE;
        }
        else
        {
            LIST_ITEM_HANDLE new_item;
            PENDING_IO* pending_socket_io = (PENDING_IO*)malloc(sizeof(PENDING_IO));
            if (pending_socket_io == NULL)
            {
                /* Codes_SRS_WSIO_01_134: [ If allocating memory for the pending IO data fails, wsio_send shall fail and return a non-zero value. ]*/
                result = MU_FAILURE;
            }
            else
            {
                /* Codes_SRS_WSIO_01_103: [ The entry shall contain the on_send_complete callback and its context. ]*/
                pending_socket_io->on_send_complete = on_send_complete;
                pending_socket_io->callback_context = callback_context;
                pending_socket_io->wsio = wsio_instance;

                /* Codes_SRS_WSIO_01_102: [ An entry shall be queued in the singly linked list by calling singlylinkedlist_add. ]*/
                if ((new_item = singlylinkedlist_add(wsio_instance->pending_io_list, pending_socket_io)) == NULL)
                {
                    /* Codes_SRS_WSIO_01_104: [ If singlylinkedlist_add fails, wsio_send shall fail and return a non-zero value. ]*/
                    free(pending_socket_io);
                    result = MU_FAILURE;
                }
                else
                {
                    /* Codes_SRS_WSIO_01_095: [ wsio_send shall call uws_client_send_frame_async, passing the buffer and size arguments as they are: ]*/
                    /* Codes_SRS_WSIO_01_097: [ The is_final argument shall be set to true. ]*/
                    /* Codes_SRS_WSIO_01_096: [ The frame type used shall be WS_FRAME_TYPE_BINARY. ]*/
                    if (uws_client_send_frame_async(wsio_instance->uws, WS_FRAME_TYPE_BINARY, (const unsigned char*)buffer, size, true, on_underlying_ws_send_frame_complete, new_item) != 0)
                    {
                        if (singlylinkedlist_remove(wsio_instance->pending_io_list, new_item) != 0)
                        {
                            LogError("Failed removing pending IO from linked list.");
                        }

                        free(pending_socket_io);
                        result = MU_FAILURE;
                    }
                    else
                    {
                        /* Codes_SRS_WSIO_01_098: [ On success, wsio_send shall return 0. ]*/
                        result = 0;
                    }
                }
            }
        }
    }

    return result;
}

void wsio_dowork(CONCRETE_IO_HANDLE ws_io)
{
    if (ws_io == NULL)
    {
        /* Codes_SRS_WSIO_01_107: [ If the ws_io argument is NULL, wsio_dowork shall do nothing. ]*/
        LogError("NULL handle");
    }
    else
    {
        WSIO_INSTANCE* wsio_instance = (WSIO_INSTANCE*)ws_io;

        /* Codes_SRS_WSIO_01_108: [ If the IO is not yet open, wsio_dowork shall do nothing. ]*/
        if (wsio_instance->io_state != IO_STATE_NOT_OPEN)
        {
            /* Codes_SRS_WSIO_01_106: [ wsio_dowork shall call uws_client_dowork with the uws handle created in wsio_create. ]*/
            uws_client_dowork(wsio_instance->uws);
        }
    }
}

int wsio_setoption(CONCRETE_IO_HANDLE ws_io, const char* optionName, const void* value)
{
    int result;
    if (
        /* Codes_SRS_WSIO_01_109: [ If any of the arguments ws_io or option_name is NULL wsio_setoption shall return a non-zero value. ]*/
        (ws_io == NULL) ||
        (optionName == NULL)
        )
    {
        LogError("Bad parameters: ws_io=%p, optionName=%p",
            ws_io, optionName);
        result = MU_FAILURE;
    }
    else
    {
        WSIO_INSTANCE* wsio_instance = (WSIO_INSTANCE*)ws_io;

        if (strcmp(WSIO_OPTIONS, optionName) == 0)
        {
            /* Codes_SRS_WSIO_01_183: [ If the option name is WSIOOptions then wsio_setoption shall call OptionHandler_FeedOptions and pass to it the underlying IO handle and the value argument. ]*/
            if (OptionHandler_FeedOptions((OPTIONHANDLER_HANDLE)value, wsio_instance->uws) != OPTIONHANDLER_OK)
            {
                /* Codes_SRS_WSIO_01_184: [ If OptionHandler_FeedOptions fails, wsio_setoption shall fail and return a non-zero value. ]*/
                LogError("unable to OptionHandler_FeedOptions");
                result = MU_FAILURE;
            }
            else
            {
                /* Codes_SRS_WSIO_01_158: [ On success, wsio_setoption shall return 0. ]*/
                result = 0;
            }
        }
        else
        {
            /* Codes_SRS_WSIO_01_156: [ Otherwise all options shall be passed as they are to uws by calling uws_client_set_option. ]*/
            if (uws_client_set_option(wsio_instance->uws, optionName, value) != 0)
            {
                /* Codes_SRS_WSIO_01_157: [ If uws_client_set_option fails, wsio_setoption shall fail and return a non-zero value. ]*/
                LogError("Setting the option %s failed", optionName);
                result = MU_FAILURE;
            }
            else
            {
                /* Codes_SRS_WSIO_01_158: [ On success, wsio_setoption shall return 0. ]*/
                result = 0;
            }
        }
    }

    return result;
}

static void* wsio_clone_option(const char* name, const void* value)
{
    void *result;

    if (
        (name == NULL) ||
        (value == NULL)
        )
    {
        /* Codes_SRS_WSIO_01_174: [ If wsio_clone_option is called with NULL name or value it shall return NULL. ]*/
        LogError("invalid argument detected: const char* name=%p, const void* value=%p", name, value);
        result = NULL;
    }
    else
    {
        if (strcmp(name, WSIO_OPTIONS) == 0)
        {
            /* Codes_SRS_WSIO_01_171: [** wsio_clone_option called with name being WSIOOptions shall return the same value. ]*/
            result = (void*)value;
        }
        else
        {
            /* Codes_SRS_WSIO_01_173: [ wsio_clone_option called with any other option name than WSIOOptions shall return NULL. ]*/
            LogError("unknown option: %s", name);
            result = NULL;
        }
    }

    return result;
}

static void wsio_destroy_option(const char* name, const void* value)
{
    if (
        (name == NULL) ||
        (value == NULL)
        )
    {
        /* Codes_SRS_WSIO_01_177: [ If wsio_destroy_option is called with NULL name or value it shall do nothing. ]*/
        LogError("Bad arguments: const char* name=%p, const void* value=%p", name, value);
    }
    else
    {
        if (strcmp(name, WSIO_OPTIONS) == 0)
        {
            /* Codes_SRS_WSIO_01_175: [ wsio_destroy_option called with the option name being WSIOOptions shall destroy the value by calling OptionHandler_Destroy. ]*/
            OptionHandler_Destroy((OPTIONHANDLER_HANDLE)value);
        }
        else
        {
            /* Codes_SRS_WSIO_01_176: [ If wsio_destroy_option is called with any other name it shall do nothing. ]*/
            LogError("unknown option: %s", name);
        }
    }
}

OPTIONHANDLER_HANDLE wsio_retrieveoptions(CONCRETE_IO_HANDLE handle)
{
    OPTIONHANDLER_HANDLE result;
    if (handle == NULL)
    {
        /* Codes_SRS_WSIO_01_118: [ If parameter handle is NULL then wsio_retrieveoptions shall fail and return NULL. ]*/
        LogError("parameter handle is NULL");
        result = NULL;
    }
    else
    {
        WSIO_INSTANCE* wsio = (WSIO_INSTANCE*)handle;

        /* Codes_SRS_WSIO_01_119: [ wsio_retrieveoptions shall call OptionHandler_Create to produce an OPTIONHANDLER_HANDLE and on success return the new OPTIONHANDLER_HANDLE handle. ]*/
        result = OptionHandler_Create(wsio_clone_option, wsio_destroy_option, wsio_setoption);
        if (result == NULL)
        {
            /* Codes_SRS_WSIO_01_120: [ If OptionHandler_Create fails then wsio_retrieveoptions shall fail and return NULL.  ]*/
            LogError("OptionHandler_Create failed");
        }
        else
        {
            /* Codes_SRS_WSIO_01_179: [ When calling uws_client_retrieve_options the uws client handle shall be passed to it. ]*/
            /* Codes_SRS_WSIO_01_178: [ uws_client_retrieve_options shall add to the option handler one option, whose name shall be uWSCLientOptions and the value shall be queried by calling uws_client_retrieve_options. ]*/
            OPTIONHANDLER_HANDLE concreteOptions = uws_client_retrieve_options(wsio->uws);
            if (concreteOptions == NULL)
            {
                /* Codes_SRS_WSIO_01_180: [ If uws_client_retrieve_options fails, uws_client_retrieve_options shall fail and return NULL. ]*/
                LogError("unable to concrete_io_retrieveoptions");
                OptionHandler_Destroy(result);
                result = NULL;
            }
            else
            {
                /* Codes_SRS_WSIO_01_181: [ Adding the option shall be done by calling OptionHandler_AddOption. ]*/
                if (OptionHandler_AddOption(result, WSIO_OPTIONS, concreteOptions) != OPTIONHANDLER_OK)
                {
                    /* Codes_SRS_WSIO_01_182: [ If OptionHandler_AddOption fails, uws_client_retrieve_options shall fail and return NULL. ]*/
                    LogError("unable to OptionHandler_AddOption");
                    OptionHandler_Destroy(concreteOptions);
                    OptionHandler_Destroy(result);
                    result = NULL;
                }
            }
        }
    }

    return result;
}

static const IO_INTERFACE_DESCRIPTION ws_io_interface_description =
{
    wsio_retrieveoptions,
    wsio_create,
    wsio_destroy,
    wsio_open,
    wsio_close,
    wsio_send,
    wsio_dowork,
    wsio_setoption
};

const IO_INTERFACE_DESCRIPTION* wsio_get_interface_description(void)
{
    return &ws_io_interface_description;
}
