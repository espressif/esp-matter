// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <time.h>
#include "tlsio_appleios.h"
#include "azure_c_shared_utility/xio.h"
#include "azure_c_shared_utility/gballoc.h"
#include "azure_c_shared_utility/xlogging.h"
#include "azure_c_shared_utility/agenttime.h"
#include "azure_c_shared_utility/singlylinkedlist.h"
#include "azure_c_shared_utility/crt_abstractions.h"
#include "azure_c_shared_utility/tlsio_options.h"


#include <CoreFoundation/CoreFoundation.h>
#include <CoreFoundation/CFError.h>
#include <CFNetwork/CFSocketStream.h>
#include <Security/SecureTransport.h>

typedef struct
{
    unsigned char* bytes;
    size_t size;
    size_t unsent_size;
    ON_SEND_COMPLETE on_send_complete;
    void* callback_context;
} PENDING_TRANSMISSION;

#define MAX_VALID_PORT 0xffff

const char WEBSOCKET_HEADER_START[] = "GET /$iothub/websocket";
const char WEBSOCKET_HEADER_NO_CERT_PARAM[] = "?iothub-no-client-cert=true";
const size_t WEBSOCKET_HEADER_START_SIZE = sizeof(WEBSOCKET_HEADER_START) - 1;
const size_t WEBSOCKET_HEADER_NO_CERT_PARAM_SIZE = sizeof(WEBSOCKET_HEADER_NO_CERT_PARAM) - 1;

// The TLSIO_RECEIVE_BUFFER_SIZE has very little effect on performance, and is kept small
// to minimize memory consumption.
#define TLSIO_RECEIVE_BUFFER_SIZE 64


typedef enum TLSIO_STATE_TAG
{
    TLSIO_STATE_CLOSED,
    TLSIO_STATE_OPENING_WAITING_DNS,
    TLSIO_STATE_OPENING_WAITING_SOCKET,
    TLSIO_STATE_OPENING_WAITING_SSL,
    TLSIO_STATE_OPEN,
    TLSIO_STATE_ERROR,
} TLSIO_STATE;

bool is_an_opening_state(TLSIO_STATE state)
{
    return state == TLSIO_STATE_OPENING_WAITING_DNS ||
        state == TLSIO_STATE_OPENING_WAITING_SOCKET ||
        state == TLSIO_STATE_OPENING_WAITING_SSL;
}

typedef struct TLS_IO_INSTANCE_TAG
{
    ON_BYTES_RECEIVED on_bytes_received;
    ON_IO_ERROR on_io_error;
    ON_IO_OPEN_COMPLETE on_open_complete;
    void* on_bytes_received_context;
    void* on_io_error_context;
    void* on_open_complete_context;
    TLSIO_STATE tlsio_state;
    CFStringRef hostname;
    uint16_t port;
    bool no_messages_yet_sent;
    CFReadStreamRef sockRead;
    CFWriteStreamRef sockWrite;
    SINGLYLINKEDLIST_HANDLE pending_transmission_list;
    TLSIO_OPTIONS options;
} TLS_IO_INSTANCE;

/* Codes_SRS_TLSIO_30_005: [ The phrase "enter TLSIO_STATE_EXT_ERROR" means the adapter shall call the on_io_error function and pass the on_io_error_context that was supplied in tlsio_open_async. ]*/
static void enter_tlsio_error_state(TLS_IO_INSTANCE* tls_io_instance)
{
    if (tls_io_instance->tlsio_state != TLSIO_STATE_ERROR)
    {
        tls_io_instance->tlsio_state = TLSIO_STATE_ERROR;
        tls_io_instance->on_io_error(tls_io_instance->on_io_error_context);
    }
}

/* Codes_SRS_TLSIO_30_005: [ When the adapter enters TLSIO_STATE_EXT_ERROR it shall call the  on_io_error function and pass the on_io_error_context that were supplied in  tlsio_open . ]*/
static void enter_open_error_state(TLS_IO_INSTANCE* tls_io_instance)
{
    // save instance variables in case the framework destroys this object before we exit
    ON_IO_OPEN_COMPLETE on_open_complete = tls_io_instance->on_open_complete;
    void* on_open_complete_context = tls_io_instance->on_open_complete_context;
    enter_tlsio_error_state(tls_io_instance);
    on_open_complete(on_open_complete_context, IO_OPEN_ERROR);
}

// Return true if a message was available to remove
static bool process_and_destroy_head_message(TLS_IO_INSTANCE* tls_io_instance, IO_SEND_RESULT send_result)
{
    bool result;
    LIST_ITEM_HANDLE head_pending_io;
    if (send_result == IO_SEND_ERROR)
    {
        /* Codes_SRS_TLSIO_30_095: [ If the send process fails before sending all of the bytes in an enqueued message, the tlsio_dowork shall call the message's on_send_complete along with its associated callback_context and IO_SEND_ERROR. ]*/
        enter_tlsio_error_state(tls_io_instance);
    }
    head_pending_io = singlylinkedlist_get_head_item(tls_io_instance->pending_transmission_list);
    if (head_pending_io != NULL)
    {
        PENDING_TRANSMISSION* head_message = (PENDING_TRANSMISSION*)singlylinkedlist_item_get_value(head_pending_io);

        if (singlylinkedlist_remove(tls_io_instance->pending_transmission_list, head_pending_io) != 0)
        {
            // This particular situation is a bizarre and unrecoverable internal error
            /* Codes_SRS_TLSIO_30_094: [ If the send process encounters an internal error or calls on_send_complete with IO_SEND_ERROR due to either failure or timeout, it shall also call on_io_error and pass in the associated on_io_error_context. ]*/
            enter_tlsio_error_state(tls_io_instance);
            LogError("Failed to remove message from list");
        }

        // on_send_complete is checked for NULL during PENDING_TRANSMISSION creation
        /* Codes_SRS_TLSIO_30_095: [ If the send process fails before sending all of the bytes in an enqueued message, the tlsio_dowork shall call the message's on_send_complete along with its associated callback_context and IO_SEND_ERROR. ]*/
        head_message->on_send_complete(head_message->callback_context, send_result);

        free(head_message->bytes);
        free(head_message);
        result = true;
    }
    else
    {
        result = false;
    }
    return result;
}

static void internal_close(TLS_IO_INSTANCE* tls_io_instance)
{
    /* Codes_SRS_TLSIO_30_009: [ The phrase "enter TLSIO_STATE_EXT_CLOSING" means the adapter shall iterate through any unsent messages in the queue and shall delete each message after calling its on_send_complete with the associated callback_context and IO_SEND_CANCELLED. ]*/
    /* Codes_SRS_TLSIO_30_006: [ The phrase "enter TLSIO_STATE_EXT_CLOSED" means the adapter shall forcibly close any existing connections then call the on_io_close_complete function and pass the on_io_close_complete_context that was supplied in tlsio_close_async. ]*/
    /* Codes_SRS_TLSIO_30_051: [ On success, if the underlying TLS does not support asynchronous closing, then the adapter shall enter TLSIO_STATE_EXT_CLOSED immediately after entering TLSIO_STATE_EX_CLOSING. ]*/
    if (tls_io_instance->tlsio_state == TLSIO_STATE_OPEN)
    {
        if (tls_io_instance->sockRead != NULL)
        {
            CFReadStreamClose(tls_io_instance->sockRead);
        }
        if (tls_io_instance->sockWrite != NULL)
        {
            CFWriteStreamClose(tls_io_instance->sockWrite);
        }
    }

    if (tls_io_instance->sockRead != NULL)
    {
        CFRelease(tls_io_instance->sockRead);
        tls_io_instance->sockRead = NULL;
    }

    // If the reader is NULL then the writer should be too but let's be thorough
    if (tls_io_instance->sockWrite != NULL)
    {
        CFRelease(tls_io_instance->sockWrite);
        tls_io_instance->sockWrite = NULL;
    }

    while (process_and_destroy_head_message(tls_io_instance, IO_SEND_CANCELLED));
    // singlylinkedlist_destroy gets called in the main destroy

    tls_io_instance->on_bytes_received = NULL;
    tls_io_instance->on_io_error = NULL;
    tls_io_instance->on_bytes_received_context = NULL;
    tls_io_instance->on_io_error_context = NULL;
    tls_io_instance->tlsio_state = TLSIO_STATE_CLOSED;
    tls_io_instance->on_open_complete = NULL;
    tls_io_instance->on_open_complete_context = NULL;
}

static void tlsio_appleios_destroy(CONCRETE_IO_HANDLE tls_io)
{
    if (tls_io == NULL)
    {
        /* Codes_SRS_TLSIO_30_020: [ If tlsio_handle is NULL, tlsio_destroy shall do nothing. ]*/
        LogError("NULL tlsio");
    }
    else
    {
        TLS_IO_INSTANCE* tls_io_instance = (TLS_IO_INSTANCE*)tls_io;
        if (tls_io_instance->tlsio_state != TLSIO_STATE_CLOSED)
        {
            /* Codes_SRS_TLSIO_30_022: [ If the adapter is in any state other than TLSIO_STATE_EX_CLOSED when tlsio_destroy is called, the adapter shall enter TLSIO_STATE_EX_CLOSING and then enter TLSIO_STATE_EX_CLOSED before completing the destroy process. ]*/
            LogError("tlsio_appleios_destroy called while not in TLSIO_STATE_CLOSED.");
            internal_close(tls_io_instance);
        }
        /* Codes_SRS_TLSIO_30_021: [ The tlsio_destroy shall release all allocated resources and then release tlsio_handle. ]*/
        if (tls_io_instance->hostname != NULL)
        {
            CFRelease(tls_io_instance->hostname);
        }

        tlsio_options_release_resources(&tls_io_instance->options);

        if (tls_io_instance->pending_transmission_list != NULL)
        {
            /* Pending messages were cleared in internal_close */
            singlylinkedlist_destroy(tls_io_instance->pending_transmission_list);
        }

        free(tls_io_instance);
    }
}

/* Codes_SRS_TLSIO_30_010: [ The tlsio_create shall allocate and initialize all necessary resources and return an instance of the tlsio_appleios_compact. ]*/
static CONCRETE_IO_HANDLE tlsio_appleios_create(void* io_create_parameters)
{
    TLS_IO_INSTANCE* result;

    if (io_create_parameters == NULL)
    {
        /* Codes_SRS_TLSIO_30_013: [ If the io_create_parameters value is NULL, tlsio_create shall log an error and return NULL. ]*/
        LogError("NULL tls_io_config");
        result = NULL;
    }
    else
    {
        /* Codes_SRS_TLSIO_30_012: [ The tlsio_create shall receive the connection configuration as a TLSIO_CONFIG* in io_create_parameters. ]*/
        TLSIO_CONFIG* tls_io_config = (TLSIO_CONFIG*)io_create_parameters;
        if (tls_io_config->hostname == NULL)
        {
            /* Codes_SRS_TLSIO_30_014: [ If the hostname member of io_create_parameters value is NULL, tlsio_create shall log an error and return NULL. ]*/
            LogError("NULL tls_io_config->hostname");
            result = NULL;
        }
        else
        {
            if (tls_io_config->port < 0 || tls_io_config->port > MAX_VALID_PORT)
            {
                /* Codes_SRS_TLSIO_30_015: [ If the port member of io_create_parameters value is less than 0 or greater than 0xffff, tlsio_create shall log an error and return NULL. ]*/
                LogError("tls_io_config->port out of range");
                result = NULL;
            }
            else
            {
                result = malloc(sizeof(TLS_IO_INSTANCE));
                if (result == NULL)
                {
                    /* Codes_SRS_TLSIO_30_011: [ If any resource allocation fails, tlsio_create shall return NULL. ]*/
                    LogError("malloc failed");
                }
                else
                {
                    memset(result, 0, sizeof(TLS_IO_INSTANCE));
                    result->port = (uint16_t)tls_io_config->port;
                    result->tlsio_state = TLSIO_STATE_CLOSED;
                    result->sockRead = NULL;
                    result->sockWrite = NULL;
                    result->hostname = NULL;
                    result->pending_transmission_list = NULL;
                    tlsio_options_initialize(&result->options, TLSIO_OPTION_BIT_TRUSTED_CERTS);
                    /* Codes_SRS_TLSIO_30_016: [ tlsio_create shall make a copy of the hostname member of io_create_parameters to allow deletion of hostname immediately after the call. ]*/
                    if (NULL == (result->hostname = CFStringCreateWithCString(NULL, tls_io_config->hostname, kCFStringEncodingUTF8)))
                    {
                        /* Codes_SRS_TLSIO_30_011: [ If any resource allocation fails, tlsio_create shall return NULL. ]*/
                        LogError("CFStringCreateWithCString failed");
                        tlsio_appleios_destroy(result);
                        result = NULL;
                    }
                    else
                    {
                        // Create the message queue
                        result->pending_transmission_list = singlylinkedlist_create();
                        if (result->pending_transmission_list == NULL)
                        {
                            /* Codes_SRS_TLSIO_30_011: [ If any resource allocation fails, tlsio_create shall return NULL. ]*/
                            LogError("Failed singlylinkedlist_create");
                            tlsio_appleios_destroy(result);
                            result = NULL;
                        }
                    }
                }
            }
        }
    }

    return (CONCRETE_IO_HANDLE)result;
}


static int tlsio_appleios_open_async(CONCRETE_IO_HANDLE tls_io,
    ON_IO_OPEN_COMPLETE on_io_open_complete, void* on_io_open_complete_context,
    ON_BYTES_RECEIVED on_bytes_received, void* on_bytes_received_context,
    ON_IO_ERROR on_io_error, void* on_io_error_context)
{

    int result;
    if (on_io_open_complete == NULL)
    {
        /* Codes_SRS_TLSIO_30_031: [ If the on_io_open_complete parameter is NULL, tlsio_open shall log an error and return FAILURE. ]*/
        LogError("Required parameter on_io_open_complete is NULL");
        result = MU_FAILURE;
    }
    else
    {
        if (tls_io == NULL)
        {
            /* Codes_SRS_TLSIO_30_030: [ If the tlsio_handle parameter is NULL, tlsio_open shall log an error and return FAILURE. ]*/
            result = MU_FAILURE;
            LogError("NULL tlsio");
        }
        else
        {
            if (on_bytes_received == NULL)
            {
                /* Codes_SRS_TLSIO_30_032: [ If the on_bytes_received parameter is NULL, tlsio_open shall log an error and return FAILURE. ]*/
                LogError("Required parameter on_bytes_received is NULL");
                result = MU_FAILURE;
            }
            else
            {
                if (on_io_error == NULL)
                {
                    /* Codes_SRS_TLSIO_30_033: [ If the on_io_error parameter is NULL, tlsio_open shall log an error and return FAILURE. ]*/
                    LogError("Required parameter on_io_error is NULL");
                    result = MU_FAILURE;
                }
                else
                {
                    TLS_IO_INSTANCE* tls_io_instance = (TLS_IO_INSTANCE*)tls_io;

                    if (tls_io_instance->tlsio_state != TLSIO_STATE_CLOSED)
                    {
                        /* Codes_SRS_TLSIO_30_037: [ If the adapter is in any state other than TLSIO_STATE_EXT_CLOSED when tlsio_open  is called, it shall log an error, and return FAILURE. ]*/
                        LogError("Invalid tlsio_state. Expected state is TLSIO_STATE_CLOSED.");
                        result = MU_FAILURE;
                    }
                    else
                    {
                        tls_io_instance->no_messages_yet_sent = true;
                        /* Codes_SRS_TLSIO_30_034: [ The tlsio_open shall store the provided on_bytes_received, on_bytes_received_context, on_io_error, on_io_error_context, on_io_open_complete, and on_io_open_complete_context parameters for later use as specified and tested per other line entries in this document. ]*/
                        tls_io_instance->on_bytes_received = on_bytes_received;
                        tls_io_instance->on_bytes_received_context = on_bytes_received_context;

                        tls_io_instance->on_io_error = on_io_error;
                        tls_io_instance->on_io_error_context = on_io_error_context;

                        tls_io_instance->on_open_complete = on_io_open_complete;
                        tls_io_instance->on_open_complete_context = on_io_open_complete_context;

                        /* Codes_SRS_TLSIO_30_035: [ On tlsio_open success the adapter shall enter TLSIO_STATE_EX_OPENING and return 0. ]*/
                        // All the real work happens in dowork
                        tls_io_instance->tlsio_state = TLSIO_STATE_OPENING_WAITING_DNS;
                        result = 0;
                    }
                }
            }
        }
        /* Codes_SRS_TLSIO_30_039: [ On failure, tlsio_open_async shall not call on_io_open_complete. ]*/
    }

    return result;
}

// This implementation does not have asynchronous close, but uses the _async name for consistencty with the spec
static int tlsio_appleios_close_async(CONCRETE_IO_HANDLE tls_io, ON_IO_CLOSE_COMPLETE on_io_close_complete, void* callback_context)
{
    int result;

    if (tls_io == NULL)
    {
        /* Codes_SRS_TLSIO_30_050: [ If the tlsio_handle parameter is NULL, tlsio_appleios_close_async shall log an error and return FAILURE. ]*/
        LogError("NULL tlsio");
        result = MU_FAILURE;
    }
    else
    {
        if (on_io_close_complete == NULL)
        {
            /* Codes_SRS_TLSIO_30_055: [ If the on_io_close_complete parameter is NULL, tlsio_appleios_close_async shall log an error and return FAILURE. ]*/
            LogError("NULL on_io_close_complete");
            result = MU_FAILURE;
        }
        else
        {
            TLS_IO_INSTANCE* tls_io_instance = (TLS_IO_INSTANCE*)tls_io;

            if (tls_io_instance->tlsio_state != TLSIO_STATE_OPEN &&
                tls_io_instance->tlsio_state != TLSIO_STATE_ERROR)
            {
                /* Codes_SRS_TLSIO_30_053: [ If the adapter is in any state other than TLSIO_STATE_EXT_OPEN or TLSIO_STATE_EXT_ERROR then tlsio_close_async shall log that tlsio_close_async has been called and then continue normally. ]*/
                // LogInfo rather than LogError because this is an unusual but not erroneous situation
                LogInfo("tlsio_appleios_close has been called when in neither TLSIO_STATE_OPEN nor TLSIO_STATE_ERROR.");
            }

            if (is_an_opening_state(tls_io_instance->tlsio_state))
            {
                /* Codes_SRS_TLSIO_30_057: [ On success, if the adapter is in TLSIO_STATE_EXT_OPENING, it shall call on_io_open_complete with the on_io_open_complete_context supplied in tlsio_open_async and IO_OPEN_CANCELLED. This callback shall be made before changing the internal state of the adapter. ]*/
                tls_io_instance->on_open_complete(tls_io_instance->on_open_complete_context, IO_OPEN_CANCELLED);
            }
            // This adapter does not support asynchronous closing
            /* Codes_SRS_TLSIO_30_056: [ On success the adapter shall enter TLSIO_STATE_EX_CLOSING. ]*/
            /* Codes_SRS_TLSIO_30_051: [ On success, if the underlying TLS does not support asynchronous closing, then the adapter shall enter TLSIO_STATE_EX_CLOSED immediately after entering TLSIO_STATE_EX_CLOSING. ]*/
            /* Codes_SRS_TLSIO_30_052: [ On success tlsio_close shall return 0. ]*/
            internal_close(tls_io_instance);
            on_io_close_complete(callback_context);
            result = 0;
        }
    }
    /* Codes_SRS_TLSIO_30_054: [ On failure, the adapter shall not call on_io_close_complete. ]*/

    return result;
}

static void dowork_read(TLS_IO_INSTANCE* tls_io_instance)
{
    // TRANSFER_BUFFER_SIZE is not very important because if the message is bigger
    // then the framework just calls dowork repeatedly until it gets everything. So
    // a bigger buffer would just use memory without buying anything.
    // Putting this buffer in a small function also allows it to exist on the stack
    // rather than adding to heap fragmentation.
    uint8_t buffer[TLSIO_RECEIVE_BUFFER_SIZE];
    CFIndex rcv_bytes;

    if (tls_io_instance->tlsio_state == TLSIO_STATE_OPEN)
    {
        CFStreamStatus read_status = CFReadStreamGetStatus(tls_io_instance->sockRead);
        if (read_status == kCFStreamStatusAtEnd || read_status == kCFStreamStatusError)
        {
            enter_tlsio_error_state(tls_io_instance);
        }
        else
        {
            while (tls_io_instance->tlsio_state == TLSIO_STATE_OPEN && CFReadStreamHasBytesAvailable(tls_io_instance->sockRead))
            {
                rcv_bytes = CFReadStreamRead(tls_io_instance->sockRead, buffer, (CFIndex)(sizeof(buffer)));

                if (rcv_bytes > 0)
                {
                    // tls_io_instance->on_bytes_received was already checked for NULL
                    // in the call to tlsio_appleios_open_async
                    /* Codes_SRS_TLSIO_30_100: [ As long as the TLS connection is able to provide received data, tlsio_dowork shall repeatedly read this data and call on_bytes_received with the pointer to the buffer containing the data, the number of bytes received, and the on_bytes_received_context. ]*/
                    tls_io_instance->on_bytes_received(tls_io_instance->on_bytes_received_context, buffer, rcv_bytes);
                }
                else if (rcv_bytes < 0)
                {
                    LogError("Communications error while reading");
                    enter_tlsio_error_state(tls_io_instance);
                }
                /* Codes_SRS_TLSIO_30_102: [ If the TLS connection receives no data then tlsio_dowork shall not call the on_bytes_received callback. ]*/
            }
        }
    }
}

static void dowork_send(TLS_IO_INSTANCE* tls_io_instance)
{
    LIST_ITEM_HANDLE first_pending_io = singlylinkedlist_get_head_item(tls_io_instance->pending_transmission_list);
    if (first_pending_io != NULL)
    {
        PENDING_TRANSMISSION* pending_message = (PENDING_TRANSMISSION*)singlylinkedlist_item_get_value(first_pending_io);
        uint8_t* buffer = ((uint8_t*)pending_message->bytes) + pending_message->size - pending_message->unsent_size;

        // Check to see if the socket will not block
        if (CFWriteStreamCanAcceptBytes(tls_io_instance->sockWrite))
        {
            CFIndex write_result = CFWriteStreamWrite(tls_io_instance->sockWrite, buffer, pending_message->unsent_size);
            if (write_result >= 0)
            {
                pending_message->unsent_size -= write_result;
                if (pending_message->unsent_size == 0)
                {
                    /* Codes_SRS_TLSIO_30_091: [ If tlsio_appleios_compact_dowork is able to send all the bytes in an enqueued message, it shall call the messages's on_send_complete along with its associated callback_context and IO_SEND_OK. ]*/
                    // The whole message has been sent successfully
                    process_and_destroy_head_message(tls_io_instance, IO_SEND_OK);
                }
                else
                {
                    /* Codes_SRS_TLSIO_30_093: [ If the TLS connection was not able to send an entire enqueued message at once, subsequent calls to tlsio_dowork shall continue to send the remaining bytes. ]*/
                    // Repeat the send on the next pass with the rest of the message
                    // This empty else compiles to nothing but helps readability
                }
            }
            else
            {
                // The write returned non-success. It may be busy, or it may be broken
                CFErrorRef write_error = CFWriteStreamCopyError(tls_io_instance->sockWrite);
                if (write_error != NULL && CFErrorGetCode(write_error) != errSSLWouldBlock)
                {
                    /* Codes_SRS_TLSIO_30_002: [ The phrase "destroy the failed message" means that the adapter shall remove the message from the queue and destroy it after calling the message's on_send_complete along with its associated callback_context and IO_SEND_ERROR. ]*/
                    /* Codes_SRS_TLSIO_30_005: [ When the adapter enters TLSIO_STATE_EXT_ERROR it shall call the  on_io_error function and pass the on_io_error_context that were supplied in  tlsio_open . ]*/
                    /* Codes_SRS_TLSIO_30_095: [ If the send process fails before sending all of the bytes in an enqueued message, tlsio_dowork shall destroy the failed message and enter TLSIO_STATE_EX_ERROR. ]*/
                    // This is an unexpected error, and we need to bail out. Probably lost internet connection.
                    LogInfo("Hard error from CFWriteStreamWrite: %ld", CFErrorGetCode(write_error));
                    process_and_destroy_head_message(tls_io_instance, IO_SEND_ERROR);
                    CFRelease(write_error);
                }
                else
                {
                    // The errSSLWouldBlock is defined as a recoverable error and should just be retried
                    LogInfo("errSSLWouldBlock on write");
                }
            }
        }
    }
    else
    {
        /* Codes_SRS_TLSIO_30_096: [ If there are no enqueued messages available, tlsio_appleios_compact_dowork shall do nothing. ]*/
    }
}

static void dowork_poll_dns(TLS_IO_INSTANCE* tls_io_instance)
{
    // Sockets on iOS do DNS lookup during open - just change the status
    tls_io_instance->tlsio_state = TLSIO_STATE_OPENING_WAITING_SOCKET;
}

static void dowork_poll_socket(TLS_IO_INSTANCE* tls_io_instance)
{
    // This will pretty much only fail if we run out of memory
    CFStreamCreatePairWithSocketToHost(NULL, tls_io_instance->hostname, tls_io_instance->port, &tls_io_instance->sockRead, &tls_io_instance->sockWrite);
    if (tls_io_instance->sockRead != NULL && tls_io_instance->sockWrite != NULL)
    {
        
        CFStringRef keys[1] = {kCFStreamPropertySocketSecurityLevel};
        CFStringRef values[1] = {kCFStreamSocketSecurityLevelNegotiatedSSL};

        CFDictionaryRef tls_io_dictionary = CFDictionaryCreate(NULL , (void *)keys , (void *)values , 1,  NULL , NULL);
        
        if (CFReadStreamSetProperty(tls_io_instance->sockRead, kCFStreamPropertySSLSettings, tls_io_dictionary))

        {
            tls_io_instance->tlsio_state = TLSIO_STATE_OPENING_WAITING_SSL;
        }
        else
        {
            LogError("Failed to set socket properties");
            enter_open_error_state(tls_io_instance);
        }
        
        CFRelease(tls_io_dictionary);
    }
    else
    {
        LogError("Unable to create socket pair");
        enter_open_error_state(tls_io_instance);
    }
}

static void dowork_poll_open_ssl(TLS_IO_INSTANCE* tls_io_instance)
{
    if (CFReadStreamOpen(tls_io_instance->sockRead) && CFWriteStreamOpen(tls_io_instance->sockWrite))
    {
        /* Codes_SRS_TLSIO_30_080: [ The tlsio_dowork shall establish a TLS connection using the hostName and port provided during tlsio_open. ]*/
        // Connect succeeded
        tls_io_instance->tlsio_state = TLSIO_STATE_OPEN;
        /* Codes_SRS_TLSIO_30_007: [ The phrase "enter TLSIO_STATE_EXT_OPEN" means the adapter shall call the on_io_open_complete function and pass IO_OPEN_OK and the on_io_open_complete_context that was supplied in tlsio_open . ]*/
        /* Codes_SRS_TLSIO_30_083: [ If tlsio_dowork successfully opens the TLS connection it shall enter TLSIO_STATE_EX_OPEN. ]*/
        tls_io_instance->on_open_complete(tls_io_instance->on_open_complete_context, IO_OPEN_OK);
    }
    else
    {
        CFErrorRef readError = CFReadStreamCopyError(tls_io_instance->sockRead);
        if (readError != NULL)
        {
            CFErrorRef writeError = CFWriteStreamCopyError(tls_io_instance->sockWrite);
            if (writeError != NULL)
            {
                LogInfo("Error opening streams - read error=%ld;write error=%ld", CFErrorGetCode(readError), CFErrorGetCode(writeError));
                CFRelease(writeError);
            }
            else
            {
                LogInfo("Error opening streams - read error=%ld", CFErrorGetCode(readError));
            }
            CFRelease(readError);
        }
        enter_open_error_state(tls_io_instance);
    }
}

static void tlsio_appleios_dowork(CONCRETE_IO_HANDLE tls_io)
{
    if (tls_io == NULL)
    {
        /* Codes_SRS_TLSIO_30_070: [ If the tlsio_handle parameter is NULL, tlsio_dowork shall do nothing except log an error. ]*/
        LogError("NULL tlsio");
    }
    else
    {
        TLS_IO_INSTANCE* tls_io_instance = (TLS_IO_INSTANCE*)tls_io;

        // This switch statement handles all of the state transitions during the opening process
        switch (tls_io_instance->tlsio_state)
        {
        case TLSIO_STATE_CLOSED:
            /* Codes_SRS_TLSIO_30_075: [ If the adapter is in TLSIO_STATE_EXT_CLOSED then  tlsio_dowork  shall do nothing. ]*/
            // Waiting to be opened, nothing to do
            break;
        case TLSIO_STATE_OPENING_WAITING_DNS:
            dowork_poll_dns(tls_io_instance);
            break;
        case TLSIO_STATE_OPENING_WAITING_SOCKET:
            dowork_poll_socket(tls_io_instance);
            break;
        case TLSIO_STATE_OPENING_WAITING_SSL:
            dowork_poll_open_ssl(tls_io_instance);
            break;
        case TLSIO_STATE_OPEN:
            dowork_read(tls_io_instance);
            dowork_send(tls_io_instance);
            break;
        case TLSIO_STATE_ERROR:
            /* Codes_SRS_TLSIO_30_071: [ If the adapter is in TLSIO_STATE_EXT_ERROR then tlsio_dowork shall do nothing. ]*/
            // There's nothing valid to do here but wait to be retried
            break;
        default:
            LogError("Unexpected internal tlsio state");
            break;
        }
    }
}

static int tlsio_appleios_setoption(CONCRETE_IO_HANDLE tls_io, const char* optionName, const void* value)
{
    TLS_IO_INSTANCE* tls_io_instance = (TLS_IO_INSTANCE*)tls_io;
    /* Codes_SRS_TLSIO_30_120: [ If the tlsio_handle parameter is NULL, tlsio_appleios_setoption shall do nothing except log an error and return FAILURE. ]*/
    int result;
    if (tls_io_instance == NULL)
    {
        LogError("NULL tlsio");
        result = MU_FAILURE;
    }
    else
    {
        /* Codes_SRS_TLSIO_30_121: [ If the optionName parameter is NULL, tlsio_appleios_setoption shall do nothing except log an error and return FAILURE. ]*/
        /* Codes_SRS_TLSIO_30_122: [ If the value parameter is NULL, tlsio_appleios_setoption shall do nothing except log an error and return FAILURE. ]*/
        /* Codes_SRS_TLSIO_OPENSSL_COMPACT_30_520 [ The tlsio_setoption shall do nothing and return FAILURE. ]*/
        TLSIO_OPTIONS_RESULT options_result = tlsio_options_set(&tls_io_instance->options, optionName, value);
        if (options_result != TLSIO_OPTIONS_RESULT_SUCCESS)
        {
            LogError("Failed tlsio_options_set");
            result = MU_FAILURE;
        }
        else
        {
            result = 0;
        }
    }
    return result;
}

static int tlsio_appleios_send_async(CONCRETE_IO_HANDLE tls_io, const void* buffer, size_t size, ON_SEND_COMPLETE on_send_complete, void* callback_context)
{
    int result;
    if (on_send_complete == NULL || tls_io == NULL || buffer == NULL || size == 0 || on_send_complete == NULL)
    {
        /* Codes_SRS_TLSIO_30_062: [ If the on_send_complete is NULL, tlsio_appleios_compact_send shall log the error and return FAILURE. ]*/
        result = MU_FAILURE;
        LogError("Invalid parameter specified: tls_io: %p, buffer: %p, size: %lu, on_send_complete: %p", tls_io, buffer, (unsigned long)size, on_send_complete);
    }
    else
    {
        TLS_IO_INSTANCE* tls_io_instance = (TLS_IO_INSTANCE*)tls_io;
        if (tls_io_instance->tlsio_state != TLSIO_STATE_OPEN)
        {
            /* Codes_SRS_TLSIO_30_060: [ If the tlsio_handle parameter is NULL, tlsio_appleios_compact_send shall log an error and return FAILURE. ]*/
            /* Codes_SRS_TLSIO_30_065: [ If tlsio_appleios_compact_open has not been called or the opening process has not been completed, tlsio_appleios_compact_send shall log an error and return FAILURE. ]*/
            result = MU_FAILURE;
            LogError("tlsio_appleios_send_async without a prior successful open");
        }
        else
        {
            PENDING_TRANSMISSION* pending_transmission = (PENDING_TRANSMISSION*)malloc(sizeof(PENDING_TRANSMISSION));
            if (pending_transmission == NULL)
            {
                /* Codes_SRS_TLSIO_30_064: [ If the supplied message cannot be enqueued for transmission, tlsio_appleios_compact_send shall log an error and return FAILURE. ]*/
                result = MU_FAILURE;
                LogError("malloc failed");
            }
            else
            {
                // For AMQP and MQTT over websockets, the interaction of the IoT Hub and the
                // Apple TLS requires hacking the websocket upgrade header with a
                // "iothub-no-client-cert=true" parameter to avoid a TLS hang.
                bool add_no_cert_url_parameter = false;
                if (tls_io_instance->no_messages_yet_sent)
                {
                    tls_io_instance->no_messages_yet_sent = false;
                    if (strncmp((const char*)buffer, WEBSOCKET_HEADER_START, WEBSOCKET_HEADER_START_SIZE) == 0)
                    {
                        add_no_cert_url_parameter = true;
                        size += WEBSOCKET_HEADER_NO_CERT_PARAM_SIZE;
                    }
                }

                /* Codes_SRS_TLSIO_30_063: [ The tlsio_appleios_compact_send shall enqueue for transmission the on_send_complete, the callback_context, the size, and the contents of buffer. ]*/
                if ((pending_transmission->bytes = (unsigned char*)malloc(size)) == NULL)
                {
                    /* Codes_SRS_TLSIO_30_064: [ If the supplied message cannot be enqueued for transmission, tlsio_appleios_compact_send shall log an error and return FAILURE. ]*/
                    LogError("malloc failed");
                    free(pending_transmission);
                    result = MU_FAILURE;
                }
                else
                {
                    pending_transmission->size = size;
                    pending_transmission->unsent_size = size;
                    pending_transmission->on_send_complete = on_send_complete;
                    pending_transmission->callback_context = callback_context;
                    if (add_no_cert_url_parameter)
                    {
                        // Insert the WEBSOCKET_HEADER_NO_CERT_PARAM after the url
                        (void)memcpy(pending_transmission->bytes, WEBSOCKET_HEADER_START, WEBSOCKET_HEADER_START_SIZE);
                        (void)memcpy(pending_transmission->bytes + WEBSOCKET_HEADER_START_SIZE, WEBSOCKET_HEADER_NO_CERT_PARAM, WEBSOCKET_HEADER_NO_CERT_PARAM_SIZE);
                        (void)memcpy(pending_transmission->bytes + WEBSOCKET_HEADER_START_SIZE + WEBSOCKET_HEADER_NO_CERT_PARAM_SIZE, buffer + WEBSOCKET_HEADER_START_SIZE, size - WEBSOCKET_HEADER_START_SIZE - WEBSOCKET_HEADER_NO_CERT_PARAM_SIZE);
                    }
                    else
                    {
                        (void)memcpy(pending_transmission->bytes, buffer, size);
                    }

                    if (singlylinkedlist_add(tls_io_instance->pending_transmission_list, pending_transmission) == NULL)
                    {
                        /* Codes_SRS_TLSIO_30_064: [ If the supplied message cannot be enqueued for transmission, tlsio_appleios_compact_send shall log an error and return FAILURE. ]*/
                        LogError("Unable to add socket to pending list.");
                        free(pending_transmission->bytes);
                        free(pending_transmission);
                        result = MU_FAILURE;
                    }
                    else
                    {
                        /* Codes_SRS_TLSIO_30_063: [ On success,  tlsio_send  shall enqueue for transmission the  on_send_complete , the  callback_context , the  size , and the contents of  buffer  and then return 0. ]*/
                        result = 0;
                        dowork_send(tls_io_instance);
                    }
                }
            }
        }
        /* Codes_SRS_TLSIO_30_066: [ On failure, on_send_complete shall not be called. ]*/
    }
    return result;
}

/* Codes_SRS_TLSIO_APPLEIOS_COMPACT_30_560: [ The  tlsio_retrieveoptions  shall do nothing and return NULL. ]*/
static OPTIONHANDLER_HANDLE tlsio_appleios_retrieveoptions(CONCRETE_IO_HANDLE tls_io)
{
    TLS_IO_INSTANCE* tls_io_instance = (TLS_IO_INSTANCE*)tls_io;
    /* Codes_SRS_TLSIO_30_160: [ If the tlsio_handle parameter is NULL, tlsio_appleios_retrieveoptions shall do nothing except log an error and return FAILURE. ]*/
    OPTIONHANDLER_HANDLE result;
    if (tls_io_instance == NULL)
    {
        LogError("NULL tlsio");
        result = NULL;
    }
    else
    {
        result = tlsio_options_retrieve_options(&tls_io_instance->options, tlsio_appleios_setoption);
    }
    return result;
}

/* Codes_SRS_TLSIO_30_008: [ The tlsio_get_interface_description shall return the VTable IO_INTERFACE_DESCRIPTION. ]*/
static const IO_INTERFACE_DESCRIPTION tlsio_appleios_interface_description =
{
    tlsio_appleios_retrieveoptions,
    tlsio_appleios_create,
    tlsio_appleios_destroy,
    tlsio_appleios_open_async,
    tlsio_appleios_close_async,
    tlsio_appleios_send_async,
    tlsio_appleios_dowork,
    tlsio_appleios_setoption
};

/* Codes_SRS_TLSIO_30_001: [ The tlsio_appleios_compact shall implement and export all the Concrete functions in the VTable IO_INTERFACE_DESCRIPTION defined in the xio.h. ]*/
const IO_INTERFACE_DESCRIPTION* tlsio_appleios_get_interface_description(void)
{
    return &tlsio_appleios_interface_description;
}

const IO_INTERFACE_DESCRIPTION* socketio_get_interface_description(void)
{
    return NULL;
}

