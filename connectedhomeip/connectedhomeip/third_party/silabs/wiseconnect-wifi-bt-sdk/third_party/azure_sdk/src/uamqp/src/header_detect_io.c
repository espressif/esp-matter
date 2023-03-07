// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>
#include "azure_macro_utils/macro_utils.h"
#include "azure_c_shared_utility/gballoc.h"
#include "azure_c_shared_utility/xlogging.h"
#include "azure_c_shared_utility/singlylinkedlist.h"
#include "azure_uamqp_c/header_detect_io.h"
#include "azure_uamqp_c/server_protocol_io.h"

static const unsigned char amqp_header_bytes[] = { 'A', 'M', 'Q', 'P', 0, 1, 0, 0 };
static const unsigned char sasl_amqp_header_bytes[] = { 'A', 'M', 'Q', 'P', 3, 1, 0, 0 };

typedef enum IO_STATE_TAG
{
    IO_STATE_NOT_OPEN,
    IO_STATE_OPENING_UNDERLYING_IO,
    IO_STATE_WAIT_FOR_HEADER,
    IO_STATE_OPENING_DETECTED_IO,
    IO_STATE_OPEN,
    IO_STATE_CLOSING,
    IO_STATE_ERROR
} IO_STATE;

typedef struct INTERNAL_HEADER_DETECT_ENTRY_TAG
{
    unsigned char* header_bytes;
    size_t header_size;
    const IO_INTERFACE_DESCRIPTION* io_interface_description;
} INTERNAL_HEADER_DETECT_ENTRY;

typedef struct CHAINED_IO_TAG
{
    XIO_HANDLE detected_io;
    ON_BYTES_RECEIVED on_bytes_received;
    void* on_bytes_received_context;
} CHAINED_IO;

typedef struct HEADER_DETECT_IO_INSTANCE_TAG
{
    XIO_HANDLE underlying_io;
    IO_STATE io_state;
    size_t header_pos;
    ON_IO_OPEN_COMPLETE on_io_open_complete;
    ON_IO_CLOSE_COMPLETE on_io_close_complete;
    ON_IO_ERROR on_io_error;
    ON_BYTES_RECEIVED on_bytes_received;
    void* on_io_open_complete_context;
    void* on_io_close_complete_context;
    void* on_io_error_context;
    void* on_bytes_received_context;
    INTERNAL_HEADER_DETECT_ENTRY* header_detect_entries;
    size_t header_detect_entry_count;
    SINGLYLINKEDLIST_HANDLE chained_io_list;
    XIO_HANDLE* last_io;
} HEADER_DETECT_IO_INSTANCE;

static void destroy_io_chain(HEADER_DETECT_IO_INSTANCE* header_detect_io)
{
    LIST_ITEM_HANDLE list_item = singlylinkedlist_get_head_item(header_detect_io->chained_io_list);
    while (list_item != NULL)
    {
        CHAINED_IO* chained_io = (CHAINED_IO*)singlylinkedlist_item_get_value(list_item);

        if (singlylinkedlist_remove(header_detect_io->chained_io_list, list_item) != 0)
        {
            LogError("Cannot remove detected IO from list");
        }

        if (chained_io != NULL)
        {
            xio_destroy(chained_io->detected_io);
            free(chained_io);
        }

        list_item = singlylinkedlist_get_head_item(header_detect_io->chained_io_list);
    }

    header_detect_io->last_io = &header_detect_io->underlying_io;
}

static void internal_close(HEADER_DETECT_IO_INSTANCE* header_detect_io)
{
    // close the last underlying IO (the one that we're talking to)
    if (xio_close(*header_detect_io->last_io, NULL, NULL) != 0)
    {
        LogError("Cannot close underlying IO");
    }

    destroy_io_chain(header_detect_io);

    header_detect_io->io_state = IO_STATE_NOT_OPEN;
}

static void on_underlying_io_open_complete(void* context, IO_OPEN_RESULT open_result);
static void on_underlying_io_bytes_received(void* context, const unsigned char* buffer, size_t size);
static void on_underlying_io_error(void* context);

static void indicate_error(HEADER_DETECT_IO_INSTANCE* header_detect_io_instance)
{
    if (header_detect_io_instance->on_io_error != NULL)
    {
        header_detect_io_instance->on_io_error(header_detect_io_instance->on_io_error_context);
    }
}

static void indicate_open_complete(HEADER_DETECT_IO_INSTANCE* header_detect_io_instance, IO_OPEN_RESULT open_result)
{
    if (header_detect_io_instance->on_io_open_complete != NULL)
    {
        header_detect_io_instance->on_io_open_complete(header_detect_io_instance->on_io_open_complete_context, open_result);
    }
}

static void indicate_close_complete(HEADER_DETECT_IO_INSTANCE* header_detect_io_instance)
{
    if (header_detect_io_instance->on_io_close_complete != NULL)
    {
        header_detect_io_instance->on_io_close_complete(header_detect_io_instance->on_io_close_complete_context);
    }
}

static void on_send_complete(void* context, IO_SEND_RESULT send_result)
{
    // able to send the header
    HEADER_DETECT_IO_INSTANCE* header_detect_io_instance = (HEADER_DETECT_IO_INSTANCE*)context;

    if (send_result != IO_SEND_OK)
    {
        // signal error
        indicate_error(header_detect_io_instance);
    }
}

// This callback usage needs to be either verified and commented or integrated into
// the state machine.
static void unchecked_on_send_complete(void* context, IO_SEND_RESULT send_result)
{
    (void)context;
    (void)send_result;
}

static void on_underlying_io_bytes_received(void* context, const unsigned char* buffer, size_t size)
{
    if (context == NULL)
    {
        /* Codes_SRS_HEADER_DETECT_IO_01_050: [ If `context` is NULL, `on_underlying_io_bytes_received` shall do nothing. ]*/
        LogError("NULL context");
    }
    else
    {
        HEADER_DETECT_IO_INSTANCE* header_detect_io_instance = (HEADER_DETECT_IO_INSTANCE*)context;

        if ((buffer == NULL) ||
            (size == 0))
        {
            switch (header_detect_io_instance->io_state)
            {
            default:
                break;

            case IO_STATE_OPEN:
                /* Codes_SRS_HEADER_DETECT_IO_01_051: [ If `buffer` is NULL or `size` is 0 while the IO is OPEN an error shall be indicated by calling `on_io_error`. ]*/
                indicate_error(header_detect_io_instance);
                break;
            }
        }
        else
        {
            while (size > 0)
            {
                switch (header_detect_io_instance->io_state)
                {
                default:
                    break;

                case IO_STATE_OPENING_UNDERLYING_IO:
                    /* Codes_SRS_HEADER_DETECT_IO_01_049: [ When `on_underlying_io_bytes_received` is called while opening the underlying IO (before the underlying open complete is received), an error shall be indicated by calling `on_io_open_complete` with `IO_OPEN_ERROR`. ]*/
                    indicate_open_complete(header_detect_io_instance, IO_OPEN_ERROR);
                    size = 0;
                    break;

                case IO_STATE_OPENING_DETECTED_IO:
                {
                    /* Codes_SRS_HEADER_DETECT_IO_01_087: [ If `on_underlying_io_bytes_received` is called while waiting for the detected IO to complete its open, the bytes shall be given to the last created IO by calling its `on_bytes_received` callback that was filled into the `on_bytes_received` member of `SERVER_PROTOCOL_IO_CONFIG`. ]*/
                    CHAINED_IO* chained_io = (CHAINED_IO*)(((unsigned char*)header_detect_io_instance->last_io) - offsetof(CHAINED_IO, detected_io));
                    (chained_io->on_bytes_received)(chained_io->on_bytes_received_context, buffer, size);
                    size = 0;
                    break;
                }

                case IO_STATE_WAIT_FOR_HEADER:
                {
                    size_t i;
                    bool has_one_match = false;

                    /* check if any of the headers matches */
                    for (i = 0; i < header_detect_io_instance->header_detect_entry_count; i++)
                    {
                        /* Codes_SRS_HEADER_DETECT_IO_01_067: [ When `on_underlying_io_bytes_received` is called while waiting for header bytes (after the underlying IO was open), the bytes shall be matched against the entries provided in the configuration passed to `header_detect_io_create`. ]*/
                        /* Codes_SRS_HEADER_DETECT_IO_01_068: [ Header bytes shall be accepted in multiple `on_underlying_io_bytes_received` calls. ]*/
                        if ((header_detect_io_instance->header_pos < header_detect_io_instance->header_detect_entries[i].header_size) &&
                            (header_detect_io_instance->header_detect_entries[i].header_bytes[header_detect_io_instance->header_pos] == buffer[0]))
                        {
                            has_one_match = true;

                            if (header_detect_io_instance->header_pos + 1 == header_detect_io_instance->header_detect_entries[i].header_size)
                            {
                                /* recognized one header */
                                if (xio_send(*header_detect_io_instance->last_io, header_detect_io_instance->header_detect_entries[i].header_bytes, header_detect_io_instance->header_detect_entries[i].header_size, on_send_complete, header_detect_io_instance) != 0)
                                {
                                    LogError("Failed sending header");
                                    header_detect_io_instance->io_state = IO_STATE_NOT_OPEN;
                                    indicate_open_complete(header_detect_io_instance, IO_OPEN_ERROR);
                                }
                                else
                                {
                                    // wait for send complete and then start the detected IO open
                                    if (header_detect_io_instance->header_detect_entries[i].io_interface_description == NULL)
                                    {
                                        header_detect_io_instance->io_state = IO_STATE_OPEN;
                                        indicate_open_complete(header_detect_io_instance, IO_OPEN_OK);
                                    }
                                    else
                                    {
                                        SERVER_PROTOCOL_IO_CONFIG server_protocol_io_config;
                                        CHAINED_IO* chained_io = (CHAINED_IO*)malloc(sizeof(CHAINED_IO));
                                        if (chained_io == NULL)
                                        {
                                            LogError("Cannot allocate memory for chained IO");
                                            internal_close(header_detect_io_instance);
                                            indicate_open_complete(header_detect_io_instance, IO_OPEN_ERROR);
                                        }
                                        else
                                        {
                                            /* Codes_SRS_HEADER_DETECT_IO_01_076: [ If no detected IO was created then the underlying IO in the `SERVER_PROTOCOL_IO_CONFIG` structure shall be set to the `underlying_io` passed in the create arguments. ]*/
                                            /* Codes_SRS_HEADER_DETECT_IO_01_075: [ The underlying IO in the `SERVER_PROTOCOL_IO_CONFIG` structure shall be set to the last detected IO that was created if any. ]*/
                                            server_protocol_io_config.underlying_io = *header_detect_io_instance->last_io;
                                            server_protocol_io_config.on_bytes_received = &chained_io->on_bytes_received;
                                            server_protocol_io_config.on_bytes_received_context = &chained_io->on_bytes_received_context;

                                            /* Codes_SRS_HEADER_DETECT_IO_01_069: [ If a header match was detected on an entry with a non-NULL io handle, a new IO associated shall be created by calling `xio_create`. ]*/
                                            /* Codes_SRS_HEADER_DETECT_IO_01_073: [ The interface description passed to `xio_create` shall be the interface description associated with the detected header. ]*/
                                            /* Codes_SRS_HEADER_DETECT_IO_01_074: [ The IO create parameters shall be a `SERVER_PROTOCOL_IO_CONFIG` structure. ]*/
                                            chained_io->detected_io = xio_create(header_detect_io_instance->header_detect_entries[i].io_interface_description, &server_protocol_io_config);
                                            if (chained_io->detected_io == NULL)
                                            {
                                                /* Codes_SRS_HEADER_DETECT_IO_01_077: [ If `xio_create` fails the header detect IO shall be closed and an error shall be indicated by calling `on_io_open_complete` with `IO_OPEN_ERROR`. ]*/
                                                LogError("Creating detected IO failed");
                                                free(chained_io);
                                                internal_close(header_detect_io_instance);
                                                indicate_open_complete(header_detect_io_instance, IO_OPEN_ERROR);
                                            }
                                            else
                                            {
                                                /* Codes_SRS_HEADER_DETECT_IO_01_086: [ The newly created IO shall be added to the chain of IOs by calling `singlylinkedlist_add`. ]*/
                                                LIST_ITEM_HANDLE new_list_item = singlylinkedlist_add(header_detect_io_instance->chained_io_list, chained_io);
                                                if (new_list_item == NULL)
                                                {
                                                    /* Codes_SRS_HEADER_DETECT_IO_01_084: [ If `singlylinkedlist_add` fails the newly created IO shall be destroyed and an error shall be indicated by calling `on_io_open_complete` with `IO_OPEN_ERROR`. ]*/
                                                    LogError("Cannot add detected IO to list");
                                                    xio_destroy(chained_io->detected_io);
                                                    free(chained_io);
                                                    internal_close(header_detect_io_instance);
                                                    indicate_open_complete(header_detect_io_instance, IO_OPEN_ERROR);
                                                }
                                                else
                                                {
                                                    /* Codes_SRS_HEADER_DETECT_IO_01_063: [ `header_detect_io_close_async` shall close the last detected IO that was created as a result of matching a header. ]*/
                                                    XIO_HANDLE* previous_last_io = header_detect_io_instance->last_io;
                                                    header_detect_io_instance->last_io = &chained_io->detected_io;

                                                    /* Codes_SRS_HEADER_DETECT_IO_01_083: [ The header detect IO shall wait for opening of the detected IO (signaled by the `on_underlying_io_open_complete`). ]*/
                                                    header_detect_io_instance->io_state = IO_STATE_OPENING_DETECTED_IO;

                                                    /* Codes_SRS_HEADER_DETECT_IO_01_078: [ The newly create IO shall be open by calling `xio_open`. ]*/
                                                    /* Codes_SRS_HEADER_DETECT_IO_01_079: [ The `on_io_open_complete` callback passed to `xio_open` shall be `on_underlying_io_open_complete`. ]*/
                                                    /* Codes_SRS_HEADER_DETECT_IO_01_080: [ The `on_bytes_received` callback passed to `xio_open` shall be `on_underlying_io_bytes_received`. ]*/
                                                    /* Codes_SRS_HEADER_DETECT_IO_01_081: [ The `on_io_error` callback passed to `xio_open` shall be `on_underlying_io_error`. ]*/
                                                    if (xio_open(chained_io->detected_io, on_underlying_io_open_complete, header_detect_io_instance, on_underlying_io_bytes_received, header_detect_io_instance, on_underlying_io_error, header_detect_io_instance) != 0)
                                                    {
                                                        /* Codes_SRS_HEADER_DETECT_IO_01_082: [ If `xio_open` fails the header detect IO shall be closed and an error shall be indicated by calling `on_io_open_complete` with `IO_OPEN_ERROR`. ]*/
                                                        LogError("Opening detected IO failed");
                                                        if (singlylinkedlist_remove(header_detect_io_instance->chained_io_list, new_list_item) != 0)
                                                        {
                                                            LogError("Cannot remove chained IO from list");
                                                        }

                                                        xio_destroy(chained_io->detected_io);
                                                        free(chained_io);
                                                        header_detect_io_instance->last_io = previous_last_io;
                                                        internal_close(header_detect_io_instance);
                                                        indicate_open_complete(header_detect_io_instance, IO_OPEN_ERROR);
                                                    }
                                                    else
                                                    {
                                                        // all OK
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }

                                break;
                            }
                        }
                    }

                    if (has_one_match)
                    {
                        if (header_detect_io_instance->io_state == IO_STATE_OPENING_DETECTED_IO)
                        {
                            header_detect_io_instance->header_pos = 0;
                        }
                        else
                        {
                            header_detect_io_instance->header_pos++;
                        }

                        size--;
                        buffer++;
                    }
                    else
                    {
                        /* all header matches failed, we can't proceed, send back to the peer the first header we know of, */
                        /* then close as per spec.  We do not care if we fail sending */
                        if (xio_send(header_detect_io_instance->underlying_io, header_detect_io_instance->header_detect_entries[0].header_bytes, header_detect_io_instance->header_detect_entries[0].header_size, unchecked_on_send_complete, NULL) != 0)
                        {
                            LogError("Failed sending header");
                        }

                        internal_close(header_detect_io_instance);
                        indicate_open_complete(header_detect_io_instance, IO_OPEN_ERROR);
                        size = 0;
                    }

                    break;
                }

                case IO_STATE_OPEN:
                    /* Codes_SRS_HEADER_DETECT_IO_01_089: [ If `on_underlying_io_bytes_received` is called while header detect IO is OPEN the bytes shall be given to the user via the `on_bytes_received` callback that was the `on_bytes_received` callback passed to `header_detect_io_open_async`. ]*/
                    /* Codes_SRS_HEADER_DETECT_IO_01_090: [ If no detected IOs were created and `on_underlying_io_bytes_received` is called while header detect IO is OPEN, the `on_bytes_received` callback passed to `header_detect_io_open_async` shall be called to indicate the bytes as received. ]*/
                    header_detect_io_instance->on_bytes_received(header_detect_io_instance->on_bytes_received_context, buffer, size);
                    size = 0;
                    break;
                }
            }
        }
    }
}

static void on_underlying_io_close_complete(void* context)
{
    HEADER_DETECT_IO_INSTANCE* header_detect_io_instance = (HEADER_DETECT_IO_INSTANCE*)context;

    switch (header_detect_io_instance->io_state)
    {
    default:
        break;

    case IO_STATE_CLOSING:
        /* Codes_SRS_HEADER_DETECT_IO_01_095: [ When `on_underlying_io_open_complete` is called when the IO is closing, it shall destroy all the detected IOs that were created. ]*/
        destroy_io_chain(header_detect_io_instance);

        header_detect_io_instance->io_state = IO_STATE_NOT_OPEN;
        indicate_close_complete(header_detect_io_instance);
        break;

    case IO_STATE_WAIT_FOR_HEADER:
    case IO_STATE_OPENING_DETECTED_IO:
    case IO_STATE_OPENING_UNDERLYING_IO:
        header_detect_io_instance->io_state = IO_STATE_NOT_OPEN;
        indicate_open_complete(header_detect_io_instance, IO_OPEN_ERROR);
        break;
    }
}

static void on_underlying_io_open_complete(void* context, IO_OPEN_RESULT open_result)
{
    if (context == NULL)
    {
        /* Codes_SRS_HEADER_DETECT_IO_01_048: [ If `context` is NULL, `on_underlying_io_open_complete` shall do nothing. ]*/
        LogError("NULL context");
    }
    else
    {
        HEADER_DETECT_IO_INSTANCE* header_detect_io_instance = (HEADER_DETECT_IO_INSTANCE*)context;

        if (open_result == IO_OPEN_OK)
        {
            switch (header_detect_io_instance->io_state)
            {
            default:
                LogError("on_io_open_complete called in unexpected state: %d", (int)header_detect_io_instance->io_state);
                break;

            case IO_STATE_OPENING_DETECTED_IO:
            case IO_STATE_OPENING_UNDERLYING_IO:
                /* Codes_SRS_HEADER_DETECT_IO_01_046: [ When `on_underlying_io_open_complete` is called with `open_result` being `IO_OPEN_OK` while OPENING, the IO shall start monitoring received bytes in order to detect headers. ]*/
                header_detect_io_instance->io_state = IO_STATE_WAIT_FOR_HEADER;
                break;
            }
        }
        else
        {
            switch (header_detect_io_instance->io_state)
            {
            default:
                LogError("on_io_open_complete called in unexpected state: %d", (int)header_detect_io_instance->io_state);
                break;

            case IO_STATE_OPENING_DETECTED_IO:
            case IO_STATE_OPENING_UNDERLYING_IO:
                /* Codes_SRS_HEADER_DETECT_IO_01_047: [ When `on_underlying_io_open_complete` is called with `open_result` being `IO_OPEN_ERROR` while OPENING, the `on_io_open_complete` callback passed to `header_detect_io_open` shall be called with `IO_OPEN_ERROR`. ]*/
                internal_close(header_detect_io_instance);

                header_detect_io_instance->io_state = IO_STATE_NOT_OPEN;
                indicate_open_complete(header_detect_io_instance, IO_OPEN_ERROR);
                break;
            }
        }
    }
}

static void on_underlying_io_error(void* context)
{
    if (context == NULL)
    {
        /* Codes_SRS_HEADER_DETECT_IO_01_058: [ If `context` is NULL, `on_underlying_io_error` shall do nothing. ]*/
        LogError("NULL context");
    }
    else
    {
        HEADER_DETECT_IO_INSTANCE* header_detect_io_instance = (HEADER_DETECT_IO_INSTANCE*)context;

        switch (header_detect_io_instance->io_state)
        {
        default:
            break;

        case IO_STATE_WAIT_FOR_HEADER:
        case IO_STATE_OPENING_DETECTED_IO:
        case IO_STATE_OPENING_UNDERLYING_IO:
            /* Tests_SRS_HEADER_DETECT_IO_01_057: [ When `on_underlying_io_error` is called while OPENING, the IO shall indicate an error by calling `on_io_open_complete` with `IO_OPEN_ERROR` and it shall close the underlying IOs. ]*/
            internal_close(header_detect_io_instance);
            indicate_open_complete(header_detect_io_instance, IO_OPEN_ERROR);
            break;

        case IO_STATE_OPEN:
            /* Codes_SRS_HEADER_DETECT_IO_01_059: [ When `on_underlying_io_error` is called while OPEN, the error should be indicated to the consumer by calling `on_io_error` and passing the `on_io_error_context` to it. ]*/
            header_detect_io_instance->io_state = IO_STATE_ERROR;
            indicate_error(header_detect_io_instance);
            break;
        }
    }
}

static CONCRETE_IO_HANDLE header_detect_io_create(void* io_create_parameters)
{
    HEADER_DETECT_IO_INSTANCE* result;

    if (io_create_parameters == NULL)
    {
        /* Codes_SRS_HEADER_DETECT_IO_01_003: [ If `io_create_parameters` is NULL, `header_detect_io_create` shall fail and return NULL. ]*/
        LogError("NULL io_create_parameters");
        result = NULL;
    }
    else
    {
        /* Codes_SRS_HEADER_DETECT_IO_01_004: [ `io_create_parameters` shall be used as `HEADER_DETECT_IO_CONFIG*`. ]*/
        HEADER_DETECT_IO_CONFIG* header_detect_io_config = (HEADER_DETECT_IO_CONFIG*)io_create_parameters;

        /* Codes_SRS_HEADER_DETECT_IO_01_005: [ If the member `header_detect_entry_count` of `HEADER_DETECT_IO_CONFIG` is 0 then `header_detect_io_create` shall fail and return NULL. ]*/
        if ((header_detect_io_config->header_detect_entry_count == 0) ||
            /* Codes_SRS_HEADER_DETECT_IO_01_006: [ If the member `header_detect_entries` is NULL then `header_detect_io_create` shall fail and return NULL. ]*/
            (header_detect_io_config->header_detect_entries == NULL) ||
            /* Codes_SRS_HEADER_DETECT_IO_01_007: [ If the member `underlying_io` is NULL then `header_detect_io_create` shall fail and return NULL. ]*/
            (header_detect_io_config->underlying_io == NULL))
        {
            LogError("Bad create parameters: header_detect_entry_count = %lu, header_detect_entries = %p, underlying_io = %p",
                (unsigned long)header_detect_io_config->header_detect_entry_count,
                header_detect_io_config->header_detect_entries,
                header_detect_io_config->underlying_io);
            result = NULL;
        }
        else
        {
            size_t i;
            bool null_io_found = false;

            for (i = 0; i < header_detect_io_config->header_detect_entry_count; i++)
            {
                /* Codes_SRS_HEADER_DETECT_IO_01_052: [ The `io` member in the in each of the `header_detect_entries` shall be allowed to be NULL. ]*/
                if (header_detect_io_config->header_detect_entries[i].header.header_bytes == NULL)
                {
                    LogError("header detect entry %u is invalid", (unsigned int)i);
                    break;
                }

                if (header_detect_io_config->header_detect_entries[i].io_interface_description == NULL)
                {
                    null_io_found = true;
                }
            }

            if (i < header_detect_io_config->header_detect_entry_count)
            {
                result = NULL;
            }
            else if (!null_io_found)
            {
                /* Codes_SRS_HEADER_DETECT_IO_01_054: [ At least one entry in `header_detect_entries` shall have IO set to NULL, otherwise `header_detect_io_create` shall fail and return NULL. ]*/
                LogError("No default header found");
                result = NULL;
            }
            else
            {
                /* Codes_SRS_HEADER_DETECT_IO_01_001: [ `header_detect_io_create` shall create a new header detect IO instance and on success it shall return a non-NULL handle to the newly created instance. ] */
                result = (HEADER_DETECT_IO_INSTANCE*)calloc(1, sizeof(HEADER_DETECT_IO_INSTANCE));
                if (result == NULL)
                {
                    /* Codes_SRS_HEADER_DETECT_IO_01_002: [ If allocating memory for the header detect IO instance fails, `header_detect_io_create` shall fail and return NULL. ]*/
                    LogError("Cannot allocate memory for header detect IO");
                }
                else
                {
                    /* Codes_SRS_HEADER_DETECT_IO_01_009: [ The `header_detect_entries` array shall be copied so that it can be later used when detecting which header was received. ]*/
                    result->header_detect_entries = (INTERNAL_HEADER_DETECT_ENTRY*)calloc(1, (header_detect_io_config->header_detect_entry_count * sizeof(INTERNAL_HEADER_DETECT_ENTRY)));
                    if (result->header_detect_entries == NULL)
                    {
                        free(result);
                        result = NULL;
                    }
                    else
                    {
                        result->header_detect_entry_count = header_detect_io_config->header_detect_entry_count;

                        /* Codes_SRS_HEADER_DETECT_IO_01_009: [ The `header_detect_entries` array shall be copied so that it can be later used when detecting which header was received. ]*/
                        for (i = 0; i < header_detect_io_config->header_detect_entry_count; i++)
                        {
                            result->header_detect_entries[i].header_size = header_detect_io_config->header_detect_entries[i].header.header_size;
                            result->header_detect_entries[i].header_bytes = (unsigned char*)malloc(result->header_detect_entries[i].header_size);
                            if (result->header_detect_entries[i].header_bytes == NULL)
                            {
                                /* Codes_SRS_HEADER_DETECT_IO_01_010: [ If allocating memory for the `header_detect_entries` or its constituents fails then `header_detect_io_create` shall fail and return NULL. ]*/
                                break;
                            }
                            else
                            {
                                /* Codes_SRS_HEADER_DETECT_IO_01_014: [ For each entry in `header_detect_entries` the `header` field shall also be copied. ]*/
                                (void)memcpy(result->header_detect_entries[i].header_bytes, header_detect_io_config->header_detect_entries[i].header.header_bytes, result->header_detect_entries[i].header_size);
                                result->header_detect_entries[i].io_interface_description = header_detect_io_config->header_detect_entries[i].io_interface_description;
                            }
                        }

                        if (i < header_detect_io_config->header_detect_entry_count)
                        {
                            size_t j;

                            LogError("Failed copying header detect configuration");
                            for (j = 0; j < i; j++)
                            {
                                free(result->header_detect_entries[j].header_bytes);
                            }

                            free(result->header_detect_entries);
                            free(result);
                            result = NULL;
                        }
                        else
                        {
                            /* Codes_SRS_HEADER_DETECT_IO_01_060: [ `header_detect_io_create` shall create a singly linked list by calling `singlylinkedlist_create` where the chained detected IOs shall be stored. ]*/
                            result->chained_io_list = singlylinkedlist_create();
                            if (result->chained_io_list == NULL)
                            {
                                /* Codes_SRS_HEADER_DETECT_IO_01_065: [ If `singlylinkedlist_create` fails then `header_detect_io_create` shall fail and return NULL. ]*/
                                LogError("Failed copying header detect configuration");
                                for (i = 0; i < result->header_detect_entry_count; i++)
                                {
                                    free(result->header_detect_entries[i].header_bytes);
                                }

                                free(result->header_detect_entries);
                                free(result);
                                result = NULL;
                            }
                            else
                            {
                                result->underlying_io = header_detect_io_config->underlying_io;
                                result->on_io_open_complete = NULL;
                                result->on_io_close_complete = NULL;
                                result->on_io_error = NULL;
                                result->on_bytes_received = NULL;
                                result->on_io_open_complete_context = NULL;
                                result->on_io_close_complete_context = NULL;
                                result->on_io_error_context = NULL;
                                result->on_bytes_received_context = NULL;

                                /* Codes_SRS_HEADER_DETECT_IO_01_070: [ If no detected IO was created then `header_detect_io_close_async` shall close the `underlying_io` passed in `header_detect_io_create`. ]*/
                                result->last_io = &result->underlying_io;

                                result->io_state = IO_STATE_NOT_OPEN;
                            }
                        }
                    }
                }
            }
        }
    }

    return result;
}

static void header_detect_io_destroy(CONCRETE_IO_HANDLE header_detect_io)
{
    if (header_detect_io == NULL)
    {
        /* Codes_SRS_HEADER_DETECT_IO_01_012: [ If `header_detect_io` is NULL, `header_detect_io_destroy` shall do nothing. ]*/
        LogError("NULL header_detect_io");
    }
    else
    {
        size_t i;
        HEADER_DETECT_IO_INSTANCE* header_detect_io_instance = (HEADER_DETECT_IO_INSTANCE*)header_detect_io;

        if (header_detect_io_instance->io_state != IO_STATE_NOT_OPEN)
        {
            /* Codes_SRS_HEADER_DETECT_IO_01_062: [ If the IO is still open when `header_detect_io_destroy` is called, all actions normally executed when closing the IO shall also be executed. ]*/
            internal_close(header_detect_io_instance);
        }

        /* Codes_SRS_HEADER_DETECT_IO_01_061: [ `header_detect_io_destroy` shall destroy the chained IO list by calling `singlylinkedlist_destroy`. ]*/
        singlylinkedlist_destroy(header_detect_io_instance->chained_io_list);

        /* Codes_SRS_HEADER_DETECT_IO_01_011: [ `header_detect_io_destroy` shall free all resources associated with the `header_detect_io` handle. ]*/
        for (i = 0; i < header_detect_io_instance->header_detect_entry_count; i++)
        {
            /* Codes_SRS_HEADER_DETECT_IO_01_013: [ `header_detect_io_destroy` shall free the memory allocated for the `header_detect_entries`. ]*/
            free(header_detect_io_instance->header_detect_entries[i].header_bytes);
        }

        free(header_detect_io_instance->header_detect_entries);

        free(header_detect_io);
    }
}

static int header_detect_io_open_async(CONCRETE_IO_HANDLE header_detect_io, ON_IO_OPEN_COMPLETE on_io_open_complete, void* on_io_open_complete_context, ON_BYTES_RECEIVED on_bytes_received, void* on_bytes_received_context, ON_IO_ERROR on_io_error, void* on_io_error_context)
{
    int result;

    if ((header_detect_io == NULL) ||
        (on_io_open_complete == NULL) ||
        (on_bytes_received == NULL) ||
        (on_io_error == NULL))
    {
        /* Codes_SRS_HEADER_DETECT_IO_01_021: [ If `header_detect_io`, `on_io_open_complete`, `on_bytes_received` or `on_io_error` is NULL, `header_detect_io_open_async` shall fail and return a non-zero value. ]*/
        LogError("Bad arguments: header_detect_io = %p, on_io_open_complete = %p, on_bytes_received = %p, on_io_error = %p",
            header_detect_io, on_io_open_complete, on_bytes_received, on_io_error);
        result = MU_FAILURE;
    }
    else
    {
        HEADER_DETECT_IO_INSTANCE* header_detect_io_instance = (HEADER_DETECT_IO_INSTANCE*)header_detect_io;

        if (header_detect_io_instance->io_state != IO_STATE_NOT_OPEN)
        {
            /* Codes_SRS_HEADER_DETECT_IO_01_020: [ If the IO is already OPEN or OPENING then `header_detect_io_open_async` shall fail and return a non-zero value. ]*/
            LogError("Already OPEN");
            result = MU_FAILURE;
        }
        else
        {
            header_detect_io_instance->on_bytes_received = on_bytes_received;
            header_detect_io_instance->on_io_open_complete = on_io_open_complete;
            header_detect_io_instance->on_io_error = on_io_error;
            header_detect_io_instance->on_bytes_received_context = on_bytes_received_context;
            header_detect_io_instance->on_io_open_complete_context = on_io_open_complete_context;
            header_detect_io_instance->on_io_error_context = on_io_error_context;

            header_detect_io_instance->io_state = IO_STATE_OPENING_UNDERLYING_IO;
            header_detect_io_instance->header_pos = 0;

            /* Codes_SRS_HEADER_DETECT_IO_01_015: [ `header_detect_io_open_async` shall open the underlying IO by calling `xio_open` and passing to it: ]*/
            /* Codes_SRS_HEADER_DETECT_IO_01_016: [ - `xio` shall be the `underlying_io` member of the `io_create_parameters` passed to `header_detect_io_create`. ]*/
            /* Codes_SRS_HEADER_DETECT_IO_01_017: [ - `on_io_open_complete`, `on_io_open_complete_context`, `on_bytes_received`, `on_bytes_received_context`, `on_error` and `on_error_context` shall be set to implementation specific values of `header_detect_io`. ]*/
            if (xio_open(header_detect_io_instance->underlying_io, on_underlying_io_open_complete, header_detect_io_instance, on_underlying_io_bytes_received, header_detect_io_instance, on_underlying_io_error, header_detect_io_instance) != 0)
            {
                /* Codes_SRS_HEADER_DETECT_IO_01_019: [ If `xio_open` fails, `header_detect_io_open_async` shall fail and return a non-zero value. ]*/
                LogError("xio_open failed");
                result = MU_FAILURE;
            }
            else
            {
                /* Codes_SRS_HEADER_DETECT_IO_01_018: [ On success `header_detect_io_open_async` shall return 0. ]*/
                result = 0;
            }
        }
    }

    return result;
}

static int header_detect_io_close_async(CONCRETE_IO_HANDLE header_detect_io, ON_IO_CLOSE_COMPLETE on_io_close_complete, void* callback_context)
{
    int result;

    if (header_detect_io == NULL)
    {
        /* Codes_SRS_HEADER_DETECT_IO_01_026: [ If `header_detect_io` is NULL, `header_detect_io_close_async` shall fail and return a non-zero value. ]*/
        LogError("NULL header_detect_io");
        result = MU_FAILURE;
    }
    else
    {
        HEADER_DETECT_IO_INSTANCE* header_detect_io_instance = (HEADER_DETECT_IO_INSTANCE*)header_detect_io;

        /* Codes_SRS_HEADER_DETECT_IO_01_027: [ If the IO is not OPEN (open has not been called or close has been completely carried out) `header_detect_io_close_async` shall fail and return a non-zero value. ]*/
        if ((header_detect_io_instance->io_state == IO_STATE_OPENING_UNDERLYING_IO) ||
            (header_detect_io_instance->io_state == IO_STATE_OPENING_DETECTED_IO) ||
            (header_detect_io_instance->io_state == IO_STATE_WAIT_FOR_HEADER))
        {
            /* Codes_SRS_HEADER_DETECT_IO_01_028: [ If the IO is OPENING (`header_detect_io_open_async` has been called, but no header has been detected yet), `header_detect_io_close_async` shall close the underlying IO and call `on_io_open_complete` with `IO_OPEN_CANCELLED`. ]*/
            (void)internal_close(header_detect_io_instance);
            header_detect_io_instance->on_io_open_complete(header_detect_io_instance->on_io_open_complete_context, IO_OPEN_CANCELLED);
            result = 0;
        }
        else if ((header_detect_io_instance->io_state == IO_STATE_NOT_OPEN) ||
            /* Codes_SRS_HEADER_DETECT_IO_01_053: [ If the IO is CLOSING then `header_detect_io_close_async` shall fail and return a non-zero value. ]*/
            (header_detect_io_instance->io_state == IO_STATE_CLOSING))
        {
            LogError("Not open");
            result = MU_FAILURE;
        }
        else
        {
            header_detect_io_instance->io_state = IO_STATE_CLOSING;
            header_detect_io_instance->on_io_close_complete = on_io_close_complete;
            header_detect_io_instance->on_io_close_complete_context = callback_context;

            /* Codes_SRS_HEADER_DETECT_IO_01_022: [ `header_detect_io_close_async` shall close the underlying IO by calling `xio_close` and passing to it: ]*/
            /* Codes_SRS_HEADER_DETECT_IO_01_023: [ - `xio` shall be the `underlying_io` member of the `io_create_parameters` passed to `header_detect_io_create`. ]*/
            /* Codes_SRS_HEADER_DETECT_IO_01_024: [ - `on_io_close_complete` shall be set to implementation specific values of `header_detect_io`. ]*/
            if (xio_close(*header_detect_io_instance->last_io, on_underlying_io_close_complete, header_detect_io_instance) != 0)
            {
                /* Codes_SRS_HEADER_DETECT_IO_01_092: [ If `xio_close` fails `header_detect_io_close_async` shall fail and return a non-zero value. ]*/
                LogError("xio_close failed");
                result = MU_FAILURE;
            }
            else
            {
                /* Codes_SRS_HEADER_DETECT_IO_01_025: [ On success `header_detect_io_close_async` shall return 0. ]*/
                result = 0;
            }
        }
    }

    return result;
}

static int header_detect_io_send_async(CONCRETE_IO_HANDLE header_detect_io, const void* buffer, size_t size, ON_SEND_COMPLETE on_send_complete, void* callback_context)
{
    int result;

    /* Codes_SRS_HEADER_DETECT_IO_01_055: [ `on_send_complete` and `callback_context` shall be allowed to be NULL. ]*/
    if ((header_detect_io == NULL) ||
        (buffer == NULL) ||
        /* Codes_SRS_HEADER_DETECT_IO_01_034: [ If `size` is 0, `header_detect_io_send_async` shall fail and return a non-zero value. ]*/
        (size == 0))
    {
        /* Codes_SRS_HEADER_DETECT_IO_01_033: [ If `header_detect_io` or `buffer` is NULL, `header_detect_io_send_async` shall fail and return a non-zero value. ]*/
        LogError("Bad arguments: header_detect_io = %p, buffer = %p, size = %u",
            header_detect_io, buffer, (unsigned int)size);
        result = MU_FAILURE;
    }
    else
    {
        HEADER_DETECT_IO_INSTANCE* header_detect_io_instance = (HEADER_DETECT_IO_INSTANCE*)header_detect_io;

        if (header_detect_io_instance->io_state != IO_STATE_OPEN)
        {
            /* Codes_SRS_HEADER_DETECT_IO_01_093: [ `header_detect_io_send_async` when the IO is not open shall fail and return a non-zero value. ]*/
            LogError("header_detect_io not OPEN");
            result = MU_FAILURE;
        }
        else
        {
            /* Codes_SRS_HEADER_DETECT_IO_01_029: [ If no detected IO was created, `header_detect_io_send_async` shall send the bytes to the underlying IO passed via `header_detect_io_create`. ]*/
            /* Codes_SRS_HEADER_DETECT_IO_01_030: [ The `buffer`, `size`, `on_send_complete` and `callback_context` shall be passed as is to `xio_send`. ]*/
            /* Codes_SRS_HEADER_DETECT_IO_01_071: [ If the header IO is open `header_detect_io_send_async` shall send the bytes to the last detected IO by calling `xio_send` that was created as result of matching a header. ]*/
            if (xio_send(*header_detect_io_instance->last_io, buffer, size, on_send_complete, callback_context) != 0)
            {
                LogError("xio_send failed");
                result = MU_FAILURE;
            }
            else
            {
                /* Codes_SRS_HEADER_DETECT_IO_01_031: [ On success `header_detect_io_send_async` shall return 0. ]*/
                result = 0;
            }
        }
    }

    return result;
}

static void header_detect_io_dowork(CONCRETE_IO_HANDLE header_detect_io)
{
    if (header_detect_io == NULL)
    {
        /* Codes_SRS_HEADER_DETECT_IO_01_036: [ If `header_detect_io` is NULL, `header_detect_io_dowork` shall do nothing. ]*/
        LogError("NULL header_detect_io");
    }
    else
    {
        HEADER_DETECT_IO_INSTANCE* header_detect_io_instance = (HEADER_DETECT_IO_INSTANCE*)header_detect_io;

        /* Codes_SRS_HEADER_DETECT_IO_01_037: [ No work shall be scheduled if `header_detect_io` is not OPEN or in ERROR (an error has been indicated to the user). ]*/
        if ((header_detect_io_instance->io_state != IO_STATE_NOT_OPEN) &&
            (header_detect_io_instance->io_state != IO_STATE_ERROR))
        {
            /* Codes_SRS_HEADER_DETECT_IO_01_056: [ `header_detect_io_dowork` shall call `xio_dowork` for all detected IOs created as a result of matching headers. ]*/
            LIST_ITEM_HANDLE list_item = singlylinkedlist_get_head_item(header_detect_io_instance->chained_io_list);
            while (list_item != NULL)
            {
                CHAINED_IO* chained_io = (CHAINED_IO*)singlylinkedlist_item_get_value(list_item);
                if (chained_io != NULL)
                {
                    xio_dowork(chained_io->detected_io);
                }

                list_item = singlylinkedlist_get_next_item(list_item);
            }

            /* Codes_SRS_HEADER_DETECT_IO_01_035: [ `header_detect_io_dowork` shall schedule work for the underlying IO associated with `header_detect_io` by calling `xio_dowork` and passing as argument the `underlying_io` member of the `io_create_parameters` passed to `header_detect_io_create`. ]*/
            xio_dowork(header_detect_io_instance->underlying_io);
        }
    }
}

static int header_detect_io_set_option(CONCRETE_IO_HANDLE header_detect_io, const char* option_name, const void* value)
{
    int result;

    if ((header_detect_io == NULL) ||
        (option_name == NULL))
    {
        /* Codes_SRS_HEADER_DETECT_IO_01_044: [ If `header_detect_io` or `optionName` is NULL, `header_detect_io_set_option` shall fail and return a non-zero value. ]*/
        LogError("NULL header_detect_io");
        result = MU_FAILURE;
    }
    else
    {
        HEADER_DETECT_IO_INSTANCE* header_detect_io_instance = (HEADER_DETECT_IO_INSTANCE*)header_detect_io;

        /* Codes_SRS_HEADER_DETECT_IO_01_042: [ If no detected IO was created `header_detect_io_set_option` shall pass any option to the underlying IO by calling `xio_setoption` and passing as IO handle the `underlying_io` member of the `io_create_parameters` passed to `header_detect_io_create`. ]*/
        /* Codes_SRS_HEADER_DETECT_IO_01_072: [ If any detected IO was created, `header_detect_io_set_option` shall pass any option to the last detected IO by calling `xio_setoption` and passing as IO handle the `underlying_io` member of the `io_create_parameters` passed to `header_detect_io_create`. ]*/
        if (xio_setoption(*header_detect_io_instance->last_io, option_name, value) != 0)
        {
            /* Codes_SRS_HEADER_DETECT_IO_01_045: [ If `xio_setoption` fails, `header_detect_io_set_option` shall fail and return a non-zero value. ]*/
            LogError("Setting the option on the underlying IO failed");
            result = MU_FAILURE;
        }
        else
        {
            /* Codes_SRS_HEADER_DETECT_IO_01_043: [ On success, `header_detect_io_set_option` shall return 0. ]*/
            result = 0;
        }
    }

    return result;
}

/*this function will clone an option given by name and value*/
static void* header_detect_io_clone_option(const char* name, const void* value)
{
    (void)name;
    (void)value;
    return NULL;
}

/*this function destroys an option previously created*/
static void header_detect_io_destroy_option(const char* name, const void* value)
{
    (void)name;
    (void)value;
}

static OPTIONHANDLER_HANDLE header_detect_io_retrieve_options(CONCRETE_IO_HANDLE header_detect_io)
{
    OPTIONHANDLER_HANDLE result;

    if (header_detect_io == NULL)
    {
        /* Codes_SRS_HEADER_DETECT_IO_01_041: [ If `header_detect_io` is NULL, `header_detect_io_retrieve_options` shall return NULL. ]*/
        LogError("NULL header_detect_io");
        result = NULL;
    }
    else
    {
        /* Codes_SRS_HEADER_DETECT_IO_01_038: [ `header_detect_io_retrieve_options` shall create a new `OPTIONHANDLER_HANDLE` by calling `OptionHandler_Create` and on success it shall return a non-NULL handle to the newly created option handler. ]*/
        result = OptionHandler_Create(header_detect_io_clone_option, header_detect_io_destroy_option, header_detect_io_set_option);
        if (result == NULL)
        {
            /* Codes_SRS_HEADER_DETECT_IO_01_040: [ If `OptionHandler_Create` fails, `header_detect_io_retrieve_options` shall return NULL. ]*/
            LogError("unable to OptionHandler_Create");
            /*return as is*/
        }
        else
        {
            /*insert here work to add the options to "result" handle*/
            /* Codes_SRS_HEADER_DETECT_IO_01_039: [ No options shall be added to the newly created option handler. ]*/
        }
    }
    return result;
}

static const IO_INTERFACE_DESCRIPTION header_detect_io_interface_description =
{
    header_detect_io_retrieve_options,
    header_detect_io_create,
    header_detect_io_destroy,
    header_detect_io_open_async,
    header_detect_io_close_async,
    header_detect_io_send_async,
    header_detect_io_dowork,
    header_detect_io_set_option
};

const IO_INTERFACE_DESCRIPTION* header_detect_io_get_interface_description(void)
{
    return &header_detect_io_interface_description;
}

static const AMQP_HEADER amqp_header =
{
    amqp_header_bytes,
    sizeof(amqp_header_bytes)
};

static const AMQP_HEADER sasl_amqp_header =
{
    sasl_amqp_header_bytes,
    sizeof(sasl_amqp_header_bytes)
};

AMQP_HEADER header_detect_io_get_amqp_header(void)
{
    /* Codes_SRS_HEADER_DETECT_IO_01_091: [ `header_detect_io_get_amqp_header` shall return a structure that should point to a buffer that contains the bytes { 'A', 'M', 'Q', 'P', 0, 1, 0, 0 }. ]*/
    return amqp_header;
}

AMQP_HEADER header_detect_io_get_sasl_amqp_header(void)
{
    /* Codes_SRS_HEADER_DETECT_IO_01_091: [ `header_detect_io_get_sasl_amqp_header` shall return a structure that should point to a buffer that contains the bytes { 'A', 'M', 'Q', 'P', 3, 1, 0, 0 }. ]*/
    return sasl_amqp_header;
}
