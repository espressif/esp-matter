// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#error  CycloneSSL is NOT supported for the Azure C shared utilities.
#error  The code here is provided for reference purposes.
#error
#error  A security audit is required if you attempt to bring this code back.

#include <stdlib.h>

/* Using tlsio_cyclonessl requires:
- Downloading the source code for CycloneSSL.
- If using cmake, defining the environment variable CycloneSSLDir
  to point to the location of the unpacked CycloneSSL source
- Using the use_cyclonessl cmake option

Alternately if cmake is not used, one has to make sure that the
CycloneSSL headers are in the include directlroy list. */

#if _WIN32
#define _WINERROR_
#include <winsock2.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include "azure_c_shared_utility/gballoc.h"
#include "azure_c_shared_utility/tlsio.h"
#include "azure_c_shared_utility/tlsio_cyclonessl.h"
#include "azure_c_shared_utility/xlogging.h"
#include "azure_c_shared_utility/crt_abstractions.h"
#include "error.h"
#include "yarrow.h"
#include "tls.h"
#include "azure_c_shared_utility/tlsio_cyclonessl_socket.h"

typedef enum TLSIO_STATE_TAG
{
    TLSIO_STATE_NOT_OPEN,
    TLSIO_STATE_OPEN,
    TLSIO_STATE_ERROR
} TLSIO_STATE;

typedef struct TLS_IO_INSTANCE_TAG
{
    ON_BYTES_RECEIVED on_bytes_received;
    ON_IO_ERROR on_io_error;
    void* on_bytes_received_context;
    void* on_io_error_context;
    TLSIO_STATE tlsio_state;
    char* hostname;
    int port;
    char* certificate;
    YarrowContext yarrowContext;
    TlsContext *tlsContext;
    TlsSocket socket;
} TLS_IO_INSTANCE;

static int tlsio_cyclonessl_close(CONCRETE_IO_HANDLE tls_io, ON_IO_CLOSE_COMPLETE on_io_close_complete, void* on_io_close_complete_context);

/*this function will clone an option given by name and value*/
static void* tlsio_cyclonessl_clone_option(const char* name, const void* value)
{
    void* result;

    /* Codes_SRS_TLSIO_CYCLONESSL_01_070: [ If the name or value arguments are NULL, tlsio_cyclonessl_clone_option shall return NULL. ]*/
    if((name == NULL) || (value == NULL))
    {
        LogError("invalid parameter detected: const char* name=%p, const void* value=%p", name, value);
        result = NULL;
    }
    else
    {
        if (strcmp(name, "TrustedCerts") == 0)
        {
            /* Codes_SRS_TLSIO_CYCLONESSL_01_071: [ tlsio_cyclonessl_clone_option shall clone the option named TrustedCerts by calling mallocAndStrcpy_s. ]*/
            if(mallocAndStrcpy_s((char**)&result, value) != 0)
            {
                LogError("unable to mallocAndStrcpy_s TrustedCerts value");
                result = NULL;
            }
            else
            {
                /* Codes_SRS_TLSIO_CYCLONESSL_01_072: [ On success it shall return a non-NULL pointer to the cloned option. ]*/
                /*return as is*/
            }
        }
        else
        {
            LogError("not handled option : %s", name);
            result = NULL;
        }
    }
    return result;
}

/*this function destroys an option previously created*/
static void tlsio_cyclonessl_destroy_option(const char* name, const void* value)
{
    /*since all options for this layer are actually string copies., disposing of one is just calling free*/

    /* Codes_SRS_TLSIO_CYCLONESSL_01_074: [ If any of the arguments is NULL, tlsio_cyclonessl_destroy_option shall do nothing. ]*/
    if ((name == NULL) || (value == NULL))
    {
        LogError("invalid parameter detected: const char* name=%p, const void* value=%p", name, value);
    }
    else
    {
        if (strcmp(name, "TrustedCerts") == 0)
        {
            free((void*)value);
        }
        else
        {
            LogError("not handled option : %s", name);
        }
    }
}

/* Codes_SRS_TLSIO_CYCLONESSL_01_001: [ tlsio_cyclonessl_create shall create a new instance of the tlsio for Cyclone SSL. ]*/
static CONCRETE_IO_HANDLE tlsio_cyclonessl_create(void* io_create_parameters)
{
    TLS_IO_INSTANCE* result;

    /* Codes_SRS_TLSIO_CYCLONESSL_01_002: [ If io_create_parameters is NULL, tlsio_cyclonessl_create shall fail and return NULL. ]*/
    if (io_create_parameters == NULL)
    {
        result = NULL;
        LogError("NULL tls_io_config.");
    }
    else
    {
        /* Codes_SRS_TLSIO_CYCLONESSL_01_003: [ io_create_parameters shall be used as a TLSIO_CONFIG\*. ]*/
        TLSIO_CONFIG* tls_io_config = io_create_parameters;

        /* Codes_SRS_TLSIO_CYCLONESSL_01_004: [ If the hostname member is NULL, then tlsio_cyclonessl_create shall fail and return NULL. ]*/
        if (tls_io_config->hostname == NULL)
        {
            result = NULL;
            LogError("NULL hostname in the TLS IO configuration.");
        }
        else
        {
            result = malloc(sizeof(TLS_IO_INSTANCE));
            if (result == NULL)
            {
                /* Codes_SRS_TLSIO_CYCLONESSL_01_076: [ If allocating memory for the TLS IO instance fails then tlsio_cyclonessl_create shall fail and return NULL. ]*/
                LogError("Failed allocating TLSIO instance.");
            }
            else
            {
                uint8_t seed[32];
                size_t i;

                /* Codes_SRS_TLSIO_CYCLONESSL_01_005: [ tlsio_cyclonessl_create shall copy the hostname and port values for later use when the open of the underlying socket is needed. ]*/
                /* Codes_SRS_TLSIO_CYCLONESSL_01_006: [ hostname shall be copied by calling mallocAndStrcpy_s. ]*/
                if (mallocAndStrcpy_s(&result->hostname, tls_io_config->hostname) != 0)
                {
                    /* Codes_SRS_TLSIO_CYCLONESSL_01_007: [ If mallocAndStrcpy_s fails then tlsio_cyclonessl_create shall fail and return NULL. ]*/
                    /* Codes_SRS_TLSIO_CYCLONESSL_01_018: [ When tlsio_cyclonessl_create fails, all allocated resources up to that point shall be freed. ]*/
                    LogError("Failed to copy the hostname.");
                    free(result);
                    result = NULL;
                }
                else
                {
                    result->port = tls_io_config->port;
                    result->certificate = NULL;
                    result->on_bytes_received = NULL;
                    result->on_bytes_received_context = NULL;
                    result->on_io_error = NULL;
                    result->on_io_error_context = NULL;
                    result->tlsio_state = TLSIO_STATE_NOT_OPEN;
                    result->socket = (TlsSocket)NULL;

                    /* seed should be initialized with some random seed ... */
                    for (i = 0; i < 32; i++)
                    {
                        seed[i] = rand() * 255 / RAND_MAX;
                    }

                    /* Codes_SRS_TLSIO_CYCLONESSL_01_008: [ tlsio_cyclonessl_create shall initialize the yarrow context by calling yarrowInit. ]*/
                    if (yarrowInit(&result->yarrowContext) != NO_ERROR)
                    {
                        /* Codes_SRS_TLSIO_CYCLONESSL_01_009: [ If yarrowInit fails then tlsio_cyclonessl_create shall return NULL. ]*/
                        /* Codes_SRS_TLSIO_CYCLONESSL_01_018: [ When tlsio_cyclonessl_create fails, all allocated resources up to that point shall be freed. ]*/
                        LogError("yarrowInit failed");
                        free(result->hostname);
                        free(result);
                        result = NULL;
                    }
                    /* Codes_SRS_TLSIO_CYCLONESSL_01_010: [ The yarrow context shall be seeded with 32 bytes of randomly chosen data by calling yarrowSeed. ]*/
                    else if (yarrowSeed(&result->yarrowContext, seed, sizeof(seed)) != NO_ERROR)
                    {
                        /* Codes_SRS_TLSIO_CYCLONESSL_01_011: [ If yarrowSeed fails then tlsio_cyclonessl_create shall return NULL. ]*/
                        /* Codes_SRS_TLSIO_CYCLONESSL_01_018: [ When tlsio_cyclonessl_create fails, all allocated resources up to that point shall be freed. ]*/
                        LogError("yarrowInit seed failed");
                        yarrowRelease(&result->yarrowContext);
                        free(result->hostname);
                        free(result);
                        result = NULL;
                    }
                    else
                    {
                        /* Codes_SRS_TLSIO_CYCLONESSL_01_012: [ tlsio_cyclonessl_create shall create a TLS context by calling tlsInit. ]*/
                        result->tlsContext = tlsInit();
                        if (result->tlsContext == NULL)
                        {
                            /* Codes_SRS_TLSIO_CYCLONESSL_01_013: [ If tlsInit fails then tlsio_cyclonessl_create shall return NULL. ]*/
                            /* Codes_SRS_TLSIO_CYCLONESSL_01_018: [ When tlsio_cyclonessl_create fails, all allocated resources up to that point shall be freed. ]*/
                            tlsFree(result->tlsContext);
                            yarrowRelease(&result->yarrowContext);
                            free(result->hostname);
                            free(result);
                            LogError("Creating the TLS context failed");
                            result = NULL;
                        }
                        /* Codes_SRS_TLSIO_CYCLONESSL_01_014: [ The TLS context shall be setup to operate as a client by calling tlsSetConnectionEnd with TLS_CONNECTION_END_CLIENT. ]*/
                        else if (tlsSetConnectionEnd(result->tlsContext, TLS_CONNECTION_END_CLIENT))
                        {
                            /* Codes_SRS_TLSIO_CYCLONESSL_01_015: [ If tlsSetConnectionEnd fails then tlsio_cyclonessl_create shall return NULL. ]*/
                            /* Codes_SRS_TLSIO_CYCLONESSL_01_018: [ When tlsio_cyclonessl_create fails, all allocated resources up to that point shall be freed. ]*/
                            tlsFree(result->tlsContext);
                            yarrowRelease(&result->yarrowContext);
                            free(result->hostname);
                            free(result);
                            LogError("tlsSetConnectionEnd failed");
                            result = NULL;
                        }
                        /* Codes_SRS_TLSIO_CYCLONESSL_01_016: [ The pseudo random number generator to be used shall be set by calling tlsSetPrng with the yarrow instance as argument. ]*/
                        else if (tlsSetPrng(result->tlsContext, YARROW_PRNG_ALGO, &result->yarrowContext) != NO_ERROR)
                        {
                            /* Codes_SRS_TLSIO_CYCLONESSL_01_017: [ If tlsSetPrng fails then tlsio_cyclonessl_create shall return NULL. ]*/
                            /* Codes_SRS_TLSIO_CYCLONESSL_01_018: [ When tlsio_cyclonessl_create fails, all allocated resources up to that point shall be freed. ]*/
                            tlsFree(result->tlsContext);
                            yarrowRelease(&result->yarrowContext);
                            free(result->hostname);
                            free(result);
                            LogError("tlsSetPrng failed");
                            result = NULL;
                        }
                    }
                }
            }
        }
    }

    return result;
}

static void tlsio_cyclonessl_destroy(CONCRETE_IO_HANDLE tls_io)
{
    /* Codes_SRS_TLSIO_CYCLONESSL_01_020: [ If tls_io is NULL, tlsio_cyclonessl_destroy shall do nothing. ]*/
    if (tls_io == NULL)
    {
        LogError("NULL tls_io.");
    }
    else
    {
        /* Codes_SRS_TLSIO_CYCLONESSL_01_019: [ tlsio_cyclonessl_destroy shall free the tlsio CycloneSSL instance. ]*/
        TLS_IO_INSTANCE* tls_io_instance = (TLS_IO_INSTANCE*)tls_io;

        /* Codes_SRS_TLSIO_CYCLONESSL_01_081: [ tlsio_cyclonessl_destroy should close the IO if it was open before freeing all the resources. ]*/
        tlsio_cyclonessl_close(tls_io, NULL, NULL);

        /* Codes_SRS_TLSIO_CYCLONESSL_01_022: [ The TLS context shall be freed by calling tlsFree. ]*/
        tlsFree(tls_io_instance->tlsContext);
        /* Codes_SRS_TLSIO_CYCLONESSL_01_021: [ tlsio_cyclonessl_destroy shall deinitialize the yarrow context by calling yarrowRelease. ]*/
        yarrowRelease(&tls_io_instance->yarrowContext);

        /* Codes_SRS_TLSIO_CYCLONESSL_01_077: [ All options cached via tlsio_cyclonessl_set_option shall also be freed. ]*/
        if (tls_io_instance->certificate != NULL)
        {
            free(tls_io_instance->certificate);
        }
        free(tls_io_instance->hostname);
        free(tls_io);
    }
}

static int tlsio_cyclonessl_open(CONCRETE_IO_HANDLE tls_io, ON_IO_OPEN_COMPLETE on_io_open_complete, void* on_io_open_complete_context, ON_BYTES_RECEIVED on_bytes_received, void* on_bytes_received_context, ON_IO_ERROR on_io_error, void* on_io_error_context)
{
    int result;

    /* Codes_SRS_TLSIO_CYCLONESSL_01_024: [ If any of the arguments tls_io, on_io_open_complete, on_bytes_received or on_io_error are NULL then tlsio_cyclonessl_open shall return a non-zero value. ]*/
    if ((tls_io == NULL) ||
        (on_io_open_complete == NULL) ||
        (on_bytes_received == NULL) ||
        (on_io_error == NULL))
    {
        result = MU_FAILURE;
        LogError("NULL tls_io.");
    }
    else
    {
        TLS_IO_INSTANCE* tls_io_instance = (TLS_IO_INSTANCE*)tls_io;

        /* Codes_SRS_TLSIO_CYCLONESSL_01_034: [ If tlsio_cyclonessl_open is called while the IO is open, tlsio_cyclonessl_open shall fail and return a non-zero value without performing any work to open the IO. ]*/
        if (tls_io_instance->tlsio_state != TLSIO_STATE_NOT_OPEN)
        {
            result = MU_FAILURE;
            LogError("Invalid tlsio_state. Expected state is TLSIO_STATE_NOT_OPEN.");
        }
        else
        {
            tls_io_instance->on_bytes_received = on_bytes_received;
            tls_io_instance->on_bytes_received_context = on_bytes_received_context;

            tls_io_instance->on_io_error = on_io_error;
            tls_io_instance->on_io_error_context = on_io_error_context;

            /* Codes_SRS_TLSIO_CYCLONESSL_01_025: [ tlsio_cyclonessl_open shall create a socket by calling tlsio_cyclonessl_socket_create, while passing to it the hostname and port that were saved in the tlsio_cyclonessl_create. ]*/
            if (tlsio_cyclonessl_socket_create(tls_io_instance->hostname, tls_io_instance->port, &tls_io_instance->socket) != 0)
            {
                /* Codes_SRS_TLSIO_CYCLONESSL_01_026: [ If tlsio_cyclonessl_socket_create fails, then tlsio_cyclonessl_open shall return a non-zero value. ]*/
                LogError("Error: Cannot open socket");
                result = MU_FAILURE;
            }
            else
            {
                /* Codes_SRS_TLSIO_CYCLONESSL_01_027: [ The socket created by tlsio_cyclonessl_socket_create shall be assigned to the TLS context by calling tlsSetSocket. ]*/
                if (tlsSetSocket(tls_io_instance->tlsContext, tls_io_instance->socket))
                {
                    /* Codes_SRS_TLSIO_CYCLONESSL_01_028: [ If tlsSetSocket fails then tlsio_cyclonessl_open shall return a non-zero value. ]*/
                    tlsio_cyclonessl_socket_destroy(tls_io_instance->socket);
                    tls_io_instance->socket = (TlsSocket)NULL;
                    LogError("Error: tlsSetSocket");
                    result = MU_FAILURE;
                }
                else
                {
                    unsigned char is_error = 0;

                    if (tls_io_instance->certificate != NULL)
                    {
                        /* Codes_SRS_TLSIO_CYCLONESSL_01_078: [ If certificates have been set by using tlsio_cyclonessl_set_option then a call to tlsSetTrustedCaList shall be made to pass the certificates to CycloneSSL. ]*/
                        if (tlsSetTrustedCaList(tls_io_instance->tlsContext, tls_io_instance->certificate, strlen(tls_io_instance->certificate)))
                        {
                            is_error = 1;
                        }
                    }

                    if (is_error)
                    {
                        /* Codes_SRS_TLSIO_CYCLONESSL_01_079: [ If tlsSetTrustedCaList fails then tlsio_cyclonessl_open shall return a non-zero value. ]*/
                        tlsio_cyclonessl_socket_destroy(tls_io_instance->socket);
                        tls_io_instance->socket = (TlsSocket)NULL;
                        LogError("tlsSetTrustedCaList failed");
                        result = MU_FAILURE;
                    }
                    else
                    {
                        /* Codes_SRS_TLSIO_CYCLONESSL_01_031: [ tlsio_cyclonessl_open shall start the TLS handshake by calling tlsConnect. ]*/
                        if (tlsConnect(tls_io_instance->tlsContext))
                        {
                            /* Codes_SRS_TLSIO_CYCLONESSL_01_032: [ If tlsConnect fails then tlsio_cyclonessl_open shall return a non-zero value. ]*/
                            tlsio_cyclonessl_socket_destroy(tls_io_instance->socket);
                            tls_io_instance->socket = (TlsSocket)NULL;
                            LogError("tlsConnect failed");
                            result = MU_FAILURE;
                        }
                        else
                        {
                            tls_io_instance->tlsio_state = TLSIO_STATE_OPEN;

                            /* Codes_SRS_TLSIO_CYCLONESSL_01_033: [ If tlsConnect succeeds, the callback on_io_open_complete shall be called, while passing on_io_open_complete_context and IO_OPEN_OK as arguments. ]*/
                            on_io_open_complete(on_io_open_complete_context, IO_OPEN_OK);

                            /* Codes_SRS_TLSIO_CYCLONESSL_01_023: [ tlsio_cyclonessl_open shall open the TLS io and on success it shall return 0. ]*/
                            result = 0;
                        }
                    }
                }
            }
        }
    }

    return result;
}

static int tlsio_cyclonessl_close(CONCRETE_IO_HANDLE tls_io, ON_IO_CLOSE_COMPLETE on_io_close_complete, void* on_io_close_complete_context)
{
    int result = 0;

    if (tls_io == NULL)
    {
        LogError("NULL tls_io.");
        result = MU_FAILURE;
    }
    else
    {
        TLS_IO_INSTANCE* tls_io_instance = (TLS_IO_INSTANCE*)tls_io;

        /* Codes_SRS_TLSIO_CYCLONESSL_01_041: [ If tlsio_cyclonessl_close is called when not open, tlsio_cyclonessl_close shall fail and return a non-zero value. ]*/
        if (tls_io_instance->tlsio_state == TLSIO_STATE_NOT_OPEN)
        {
            result = MU_FAILURE;
            LogError("Invalid tlsio_state. Expected state is TLSIO_STATE_NOT_OPEN or TLSIO_STATE_CLOSING.");
        }
        else
        {
            /* Codes_SRS_TLSIO_CYCLONESSL_01_037: [ tlsio_cyclonessl_close shall close the TLS connection by calling tlsShutdown. ]*/
            if (tlsShutdown(tls_io_instance->tlsContext))
            {
                /* Codes_SRS_TLSIO_CYCLONESSL_01_038: [ If tlsShutdown fails, tlsio_cyclonessl_close shall fail and return a non-zero value. ]*/
                LogError("tlsShutdown failed\r\n");
                result = MU_FAILURE;
            }
            else
            {
                /* Codes_SRS_TLSIO_CYCLONESSL_01_039: [ tlsio_cyclonessl_destroy shall destroy the underlying socket by calling tlsio_cyclonessl_socket_destroy. ]*/
                tlsio_cyclonessl_socket_destroy(tls_io_instance->socket);
                tls_io_instance->socket = (TlsSocket)NULL;
                tls_io_instance->tlsio_state = TLSIO_STATE_NOT_OPEN;

                /* Codes_SRS_TLSIO_CYCLONESSL_01_040: [ On success, on_io_close_complete shall be called while passing as argument on_io_close_complete_context. ]*/
                if (on_io_close_complete != NULL)
                {
                    on_io_close_complete(on_io_close_complete_context);
                }

                /* Codes_SRS_TLSIO_CYCLONESSL_01_035: [ tlsio_cyclonessl_close shall close the TLS IO and on success it shall return 0. ]*/
                result = 0;
            }
        }
    }

    return result;
}

static int tlsio_cyclonessl_send(CONCRETE_IO_HANDLE tls_io, const void* buffer, size_t size, ON_SEND_COMPLETE on_send_complete, void* on_send_complete_context)
{
    int result;

    /* Codes_SRS_TLSIO_CYCLONESSL_01_043: [ If any of the arguments tls_io or buffer is NULL, tlsio_cyclonessl_send shall fail and return a non-zero value. ]*/
    if ((tls_io == NULL) ||
        (buffer == NULL) ||
        /* Codes_SRS_TLSIO_CYCLONESSL_01_044: [ If size is 0, tlsio_cyclonessl_send shall fail and return a non-zero value. ]*/
        (size == 0))
    {
        LogError("NULL tls_io.");
        result = MU_FAILURE;
    }
    else
    {
        TLS_IO_INSTANCE* tls_io_instance = (TLS_IO_INSTANCE*)tls_io;

        /* Codes_SRS_TLSIO_CYCLONESSL_01_048: [ If tlsio_cyclonessl_send is called when the IO is not open, tlsio_cyclonessl_send shall fail and return a non-zero value. ]*/
        /* Codes_SRS_TLSIO_CYCLONESSL_01_049: [ If the IO is in an error state (an error was reported through the on_io_error callback), tlsio_cyclonessl_send shall fail and return a non-zero value. ]*/
        /* Codes_SRS_TLSIO_CYCLONESSL_01_056: [ Also the IO shall be considered in error and any subsequent calls to tlsio_cyclonessl_send shall fail. ]*/
        if (tls_io_instance->tlsio_state != TLSIO_STATE_OPEN)
        {
            LogError("Invalid tlsio_state. Expected state is TLSIO_STATE_OPEN.");
            result = MU_FAILURE;
        }
        else
        {
            /* Codes_SRS_TLSIO_CYCLONESSL_01_047: [ tlsio_cyclonessl_send shall send the bytes by calling tlsWrite and passing buffer and size as arguments. 0 shall be passed for the flags argument. ]*/
            if (tlsWrite(tls_io_instance->tlsContext, buffer, size, 0) != 0)
            {
                LogError("SSL_write error.");
                result = MU_FAILURE;
            }
            else
            {
                /* Codes_SRS_TLSIO_CYCLONESSL_01_045: [ on_send_complete shall be allowed to be NULL. ]*/
                if (on_send_complete != NULL)
                {
                    /* Codes_SRS_TLSIO_CYCLONESSL_01_046: [ On success, if a non-NULL value was passed for on_send_complete, then on_send_complete shall be called while passing to it the on_send_complete_context value. ]*/
                    on_send_complete(on_send_complete_context, IO_SEND_OK);
                }

                /* Codes_SRS_TLSIO_CYCLONESSL_01_042: [ tlsio_cyclonessl_send shall send the size bytes pointed to by buffer and on success it shall return 0. ]*/
                result = 0;
            }
        }
    }

    return result;
}

static void tlsio_cyclonessl_dowork(CONCRETE_IO_HANDLE tls_io)
{
    /* Codes_SRS_TLSIO_CYCLONESSL_01_051: [ If the tls_io argument is NULL, tlsio_cyclonessl_dowork shall do nothing. ]*/
    if (tls_io == NULL)
    {
        LogError("NULL tls_io.");
    }
    else
    {
        TLS_IO_INSTANCE* tls_io_instance = (TLS_IO_INSTANCE*)tls_io;

        /* Codes_SRS_TLSIO_CYCLONESSL_01_052: [ If the IO is not open (no open has been called or the IO has been closed) then tlsio_cyclonessl_dowork shall do nothing. ]*/
        if ((tls_io_instance->tlsio_state != TLSIO_STATE_NOT_OPEN) &&
            (tls_io_instance->tlsio_state != TLSIO_STATE_ERROR))
        {
            unsigned char buffer[64];

            size_t received;

            /* Codes_SRS_TLSIO_CYCLONESSL_01_050: [ tlsio_cyclonessl_dowork shall check if any bytes are available to be read from the CycloneSSL library and indicate those bytes as received. ]*/
            /* Codes_SRS_TLSIO_CYCLONESSL_01_053: [ If the IO is open, tlsio_cyclonessl_dowork shall attempt to read 64 bytes from the TLS library by calling tlsRead. ]*/
            /* Codes_SRS_TLSIO_CYCLONESSL_01_054: [ The flags argument for tlsRead shall be 0. ]*/
            if (tlsRead(tls_io_instance->tlsContext, buffer, sizeof(buffer), &received, 0) != 0)
            {
                /* Codes_SRS_TLSIO_CYCLONESSL_01_055: [ If tlsRead fails, the error shall be indicated by calling the on_io_error callback passed in tlsio_cyclonessl_open, while passing the on_io_error_context to the callback. ]*/
                LogError("Error received bytes");

                /* Codes_SRS_TLSIO_CYCLONESSL_01_055: [ If tlsRead fails, the error shall be indicated by calling the on_io_error callback passed in tlsio_cyclonessl_open, while passing the on_io_error_context to the callback. ]*/
                tls_io_instance->tlsio_state = TLSIO_STATE_ERROR;
                tls_io_instance->on_io_error(tls_io_instance->on_io_error_context);
            }
            else
            {
                if (received > 0)
                {
                    /* Codes_SRS_TLSIO_CYCLONESSL_01_080: [ If any bytes are read from CycloneSSL they should be indicated via the on_bytes_received callback passed to tlsio_cyclonessl_open. ]*/
                    tls_io_instance->on_bytes_received(tls_io_instance->on_bytes_received_context, buffer, received);
                }
            }
        }
    }
}

static int tlsio_cyclonessl_setoption(CONCRETE_IO_HANDLE tls_io, const char* optionName, const void* value)
{
    int result;

    /* Tests_SRS_TLSIO_CYCLONESSL_01_057: [ If any of the arguments tls_io or option_name is NULL tlsio_cyclonessl_setoption shall return a non-zero value. ]*/
    if ((tls_io == NULL) || (optionName == NULL))
    {
        LogError("NULL tls_io");
        result = MU_FAILURE;
    }
    else
    {
        TLS_IO_INSTANCE* tls_io_instance = (TLS_IO_INSTANCE*)tls_io;

        /* Codes_SRS_TLSIO_CYCLONESSL_01_060: [ - "TrustedCerts" - a char\* that shall be saved by tlsio_cyclonessl as it shall be given to the underlying CycloneSSL TLS context when the IO is open. ]*/
        if (strcmp("TrustedCerts", optionName) == 0)
        {
            const char* cert = (const char*)value;

            /* Codes_SRS_TLSIO_CYCLONESSL_01_062: [ If a previous TrustedCerts option was saved, then the previous value shall be freed. ]*/
            if (tls_io_instance->certificate != NULL)
            {
                // Free the memory if it has been previously allocated
                free(tls_io_instance->certificate);
                tls_io_instance->certificate = NULL;
            }

            /* Codes_SRS_TLSIO_CYCLONESSL_01_063: [ A NULL value shall be allowed for TrustedCerts, in which case the previously stored TrustedCerts option value shall be cleared. ]*/
            if (cert == NULL)
            {
                result = 0;
            }
            else
            {
                // Store the certificate
                if (mallocAndStrcpy_s(&tls_io_instance->certificate, cert) != 0)
                {
                    /* Codes_SRS_TLSIO_CYCLONESSL_01_061: [ If copying the char\* passed in value fails then tlsio_cyclonessl_setoption shall return a non-zero value. ]*/
                    LogError("Error allocating memory for certificates");
                    result = MU_FAILURE;
                }
                else
                {
                    /* Codes_SRS_TLSIO_CYCLONESSL_01_059: [ If the option was handled by tlsio_cyclonessl, then tlsio_cyclonessl_setoption shall return 0. ]*/
                    result = 0;
                }
            }
        }
        else
        {
            /* Codes_SRS_TLSIO_CYCLONESSL_01_058: [ If the option_name argument indicates an option that is not handled by tlsio_cyclonessl, then tlsio_cyclonessl_setoption shall return a non-zero value. ]*/
            LogError("Unrecognized option");
            result = MU_FAILURE;
        }
    }

    return result;
}

static OPTIONHANDLER_HANDLE tlsio_cyclonessl_retrieve_options(CONCRETE_IO_HANDLE handle)
{
    OPTIONHANDLER_HANDLE result;

    /* Codes_SRS_TLSIO_CYCLONESSL_01_064: [ If parameter handle is NULL then tlsio_cyclonessl_retrieve_options shall fail and return NULL. ]*/
    if (handle == NULL)
    {
        LogError("invalid parameter detected: CONCRETE_IO_HANDLE handle=%p", handle);
        result = NULL;
    }
    else
    {
        /* Codes_SRS_TLSIO_CYCLONESSL_01_065: [ tlsio_cyclonessl_retrieve_options shall produce an OPTIONHANDLER_HANDLE. ]*/
        result = OptionHandler_Create(tlsio_cyclonessl_clone_option, tlsio_cyclonessl_destroy_option, tlsio_cyclonessl_setoption);
        if (result == NULL)
        {
            /* Codes_SRS_TLSIO_CYCLONESSL_01_068: [ If producing the OPTIONHANDLER_HANDLE fails then tlsio_cyclonessl_retrieve_options shall fail and return NULL. ]*/
            LogError("unable to OptionHandler_Create");
            /*return as is*/
        }
        else
        {
            TLS_IO_INSTANCE* tls_io_instance = (TLS_IO_INSTANCE*)handle;

            /* Codes_SRS_TLSIO_CYCLONESSL_01_066: [ tlsio_cyclonessl_retrieve_options shall add to it the options: ]*/
            if (
                (tls_io_instance->certificate != NULL) &&
                /* Codes_SRS_TLSIO_CYCLONESSL_01_067: [  - TrustedCerts ]*/
                (OptionHandler_AddOption(result, "TrustedCerts", tls_io_instance->certificate) != OPTIONHANDLER_OK)
                )
            {
                LogError("unable to save TrustedCerts option");
                OptionHandler_Destroy(result);
                result = NULL;
            }
            else
            {
                /*all is fine, all interesting options have been saved*/
                /*return as is*/
            }
        }
    }
    return result;
}

static const IO_INTERFACE_DESCRIPTION tlsio_cyclonessl_interface_description =
{
    tlsio_cyclonessl_retrieve_options,
    tlsio_cyclonessl_create,
    tlsio_cyclonessl_destroy,
    tlsio_cyclonessl_open,
    tlsio_cyclonessl_close,
    tlsio_cyclonessl_send,
    tlsio_cyclonessl_dowork,
    tlsio_cyclonessl_setoption
};

/* Codes_SRS_TLSIO_CYCLONESSL_01_069: [ tlsio_cyclonessl_get_interface_description shall return a pointer to an IO_INTERFACE_DESCRIPTION structure that contains pointers to the functions: tlsio_cyclonessl_retrieve_options, tlsio_cyclonessl_create, tlsio_cyclonessl_destroy, tlsio_cyclonessl_open, tlsio_cyclonessl_close, tlsio_cyclonessl_send and tlsio_cyclonessl_dowork.  ]*/
const IO_INTERFACE_DESCRIPTION* tlsio_cyclonessl_get_interface_description(void)
{
    return &tlsio_cyclonessl_interface_description;
}
