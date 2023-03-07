// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>
#include "openssl/ssl.h"
#include "openssl/err.h"
#include "openssl/crypto.h"
#include "openssl/opensslv.h"
#include "openssl/engine.h"
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include "azure_c_shared_utility/lock.h"
#include "azure_c_shared_utility/tlsio.h"
#include "azure_c_shared_utility/tlsio_openssl.h"
#include "azure_c_shared_utility/socketio.h"
#include "azure_c_shared_utility/optimize_size.h"
#include "azure_c_shared_utility/xlogging.h"
#include "azure_c_shared_utility/crt_abstractions.h"
#include "azure_c_shared_utility/x509_openssl.h"
#include "azure_c_shared_utility/shared_util_options.h"
#include "azure_c_shared_utility/gballoc.h"
#include "azure_c_shared_utility/const_defines.h"

typedef enum TLSIO_STATE_TAG
{
    TLSIO_STATE_NOT_OPEN,
    TLSIO_STATE_OPENING_UNDERLYING_IO,
    TLSIO_STATE_IN_HANDSHAKE,
    // TLSIO_STATE_HANDSHAKE_FAILED is an ephemeral state signalling successful socket
    // operation but with rejected handshake. The tlsio will never be in this state
    // at the start of any of the API calls.
    TLSIO_STATE_HANDSHAKE_FAILED,
    TLSIO_STATE_OPEN,
    TLSIO_STATE_CLOSING,
    TLSIO_STATE_ERROR
} TLSIO_STATE;

typedef enum TLSIO_VERSION_TAG
{
    VERSION_1_0,
    VERSION_1_1,
    VERSION_1_2,
} TLSIO_VERSION;

static bool is_an_opening_state(TLSIO_STATE state)
{
    // TLSIO_STATE_HANDSHAKE_FAILED is deliberately not one of these states.
    return state == TLSIO_STATE_OPENING_UNDERLYING_IO ||
        state == TLSIO_STATE_IN_HANDSHAKE;
}

typedef int(*TLS_CERTIFICATE_VALIDATION_CALLBACK)(X509_STORE_CTX*, void*);

typedef struct TLS_IO_INSTANCE_TAG
{
    XIO_HANDLE underlying_io;
    ON_BYTES_RECEIVED on_bytes_received;
    ON_IO_OPEN_COMPLETE on_io_open_complete;
    ON_IO_CLOSE_COMPLETE on_io_close_complete;
    ON_IO_ERROR on_io_error;
    void* on_bytes_received_context;
    void* on_io_open_complete_context;
    void* on_io_close_complete_context;
    void* on_io_error_context;
    SSL* ssl;
    SSL_CTX* ssl_context;
    BIO* in_bio;
    BIO* out_bio;
    TLSIO_STATE tlsio_state;
    char* certificate;
    char* cipher_list;
    const char* x509_certificate;
    const char* x509_private_key;
    TLSIO_VERSION tls_version;
    TLS_CERTIFICATE_VALIDATION_CALLBACK tls_validation_callback;
    void* tls_validation_callback_data;
    char* hostname;
    char* engine_id;
    bool ignore_host_name_check;
    ENGINE* engine;
    OPTION_OPENSSL_KEY_TYPE x509_private_key_type;
} TLS_IO_INSTANCE;

struct CRYPTO_dynlock_value
{
    LOCK_HANDLE lock;
};

static const char* const OPTION_UNDERLYING_IO_OPTIONS = "underlying_io_options";
#define SSL_DO_HANDSHAKE_SUCCESS 1


/*this function will clone an option given by name and value*/
static void* tlsio_openssl_CloneOption(const char* name, const void* value)
{
    void* result;
    if (
        (name == NULL) || (value == NULL)
        )
    {
        LogError("invalid parameter detected: const char* name=%p, const void* value=%p", name, value);
        result = NULL;
    }
    else
    {
        if (strcmp(name, OPTION_UNDERLYING_IO_OPTIONS) == 0)
        {
            result = (void*)value;
        }
        else if (strcmp(name, OPTION_TRUSTED_CERT) == 0)
        {
            if (mallocAndStrcpy_s((char**)&result, value) != 0)
            {
                LogError("unable to mallocAndStrcpy_s TrustedCerts value");
                result = NULL;
            }
            else
            {
                /*return as is*/
            }
        }
        else if (strcmp(name, OPTION_OPENSSL_CIPHER_SUITE) == 0)
        {
            if (mallocAndStrcpy_s((char**)&result, value) != 0)
            {
                LogError("unable to mallocAndStrcpy_s CipherSuite value");
                result = NULL;
            }
            else
            {
                /*return as is*/
            }
        }
        else if (strcmp(name, SU_OPTION_X509_CERT) == 0)
        {
            if (mallocAndStrcpy_s((char**)&result, value) != 0)
            {
                LogError("unable to mallocAndStrcpy_s x509certificate value");
                result = NULL;
            }
            else
            {
                /*return as is*/
            }
        }
        else if (strcmp(name, SU_OPTION_X509_PRIVATE_KEY) == 0)
        {
            if (mallocAndStrcpy_s((char**)&result, value) != 0)
            {
                LogError("unable to mallocAndStrcpy_s x509privatekey value");
                result = NULL;
            }
            else
            {
                /*return as is*/
            }
        }
        else if (strcmp(name, OPTION_X509_ECC_CERT) == 0)
        {
            if (mallocAndStrcpy_s((char**)&result, value) != 0)
            {
                LogError("unable to mallocAndStrcpy_s x509EccCertificate value");
                result = NULL;
            }
            else
            {
                /*return as is*/
            }
        }
        else if (strcmp(name, OPTION_X509_ECC_KEY) == 0)
        {
            if (mallocAndStrcpy_s((char**)&result, value) != 0)
            {
                LogError("unable to mallocAndStrcpy_s x509EccKey value");
                result = NULL;
            }
            else
            {
                /*return as is*/
            }
        }
        else if (strcmp(name, OPTION_TLS_VERSION) == 0)
        {
            int int_value;

            if (*(TLSIO_VERSION*)value == VERSION_1_0)
            {
                int_value = 10;
            }
            else if (*(TLSIO_VERSION*)value == VERSION_1_1)
            {
                int_value = 11;
            }
            else if (*(TLSIO_VERSION*)value == VERSION_1_2)
            {
                int_value = 12;
            }
            else
            {
                LogError("Unexpected TLS version value (%d)", *(int*)value);
                int_value = -1;
            }

            if (int_value < 0)
            {
                result = NULL;
            }
            else
            {
                int* value_clone;

                if ((value_clone = (int*)malloc(sizeof(int))) == NULL)
                {
                    LogError("Failed cloning tls_version option");
                }
                else
                {
                    *value_clone = int_value;
                }

                result = value_clone;
            }
        }
        else if (
            (strcmp(name, "tls_validation_callback") == 0) ||
            (strcmp(name, "tls_validation_callback_data") == 0)
            )
        {
            result = (void*)value;
        }
        else if (strcmp(name, OPTION_OPENSSL_ENGINE) == 0)
        {
            if (mallocAndStrcpy_s((char**)&result, value) != 0)
            {
                LogError("unable to mallocAndStrcpy_s Engine value");
                result = NULL;
            }
            else
            {
                /*return as is*/
            }
        }
        else if (strcmp(name, OPTION_OPENSSL_PRIVATE_KEY_TYPE) == 0)
        {
            OPTION_OPENSSL_KEY_TYPE key_type_value = *((OPTION_OPENSSL_KEY_TYPE*)value);
            OPTION_OPENSSL_KEY_TYPE* value_clone;

            if ((value_clone = (OPTION_OPENSSL_KEY_TYPE*)malloc(sizeof(OPTION_OPENSSL_KEY_TYPE))) == NULL)
            {
                LogError("Failed cloning cert or private key option");
            }
            else
            {
                *value_clone = key_type_value;
            }

            result = value_clone;
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
static void tlsio_openssl_DestroyOption(const char* name, const void* value)
{
    /*since all options for this layer are actually string copies., disposing of one is just calling free*/
    if (
        (name == NULL) || (value == NULL)
        )
    {
        LogError("invalid parameter detected: const char* name=%p, const void* value=%p", name, value);
    }
    else
    {
        if (
            (strcmp(name, OPTION_TRUSTED_CERT) == 0) ||
            (strcmp(name, OPTION_OPENSSL_CIPHER_SUITE) == 0) ||
            (strcmp(name, SU_OPTION_X509_CERT) == 0) ||
            (strcmp(name, SU_OPTION_X509_PRIVATE_KEY) == 0) ||
            (strcmp(name, OPTION_X509_ECC_CERT) == 0) ||
            (strcmp(name, OPTION_X509_ECC_KEY) == 0) ||
            (strcmp(name, OPTION_TLS_VERSION) == 0) || 
            (strcmp(name, OPTION_OPENSSL_ENGINE) == 0) || 
            (strcmp(name, OPTION_OPENSSL_PRIVATE_KEY_TYPE) == 0)
           )
        {
            free((void*)value);
        }
        else if (
            (strcmp(name, "tls_validation_callback") == 0) ||
            (strcmp(name, "tls_validation_callback_data") == 0)
            )
        {
            // nothing to free.
        }
        else if (strcmp(name, OPTION_UNDERLYING_IO_OPTIONS) == 0)
        {
            OptionHandler_Destroy((OPTIONHANDLER_HANDLE)value);
        }
        else
        {
            LogError("not handled option : %s", name);
        }
    }
}

static OPTIONHANDLER_HANDLE tlsio_openssl_retrieveoptions(CONCRETE_IO_HANDLE handle)
{
    OPTIONHANDLER_HANDLE result;
    if (handle == NULL)
    {
        LogError("invalid parameter detected: CONCRETE_IO_HANDLE handle=%p", handle);
        result = NULL;
    }
    else
    {
        result = OptionHandler_Create(tlsio_openssl_CloneOption, tlsio_openssl_DestroyOption, tlsio_openssl_setoption);
        if (result == NULL)
        {
            LogError("unable to OptionHandler_Create");
            /*return as is*/
        }
        else
        {
            /*this layer cares about the certificates and the x509 credentials*/
            TLS_IO_INSTANCE* tls_io_instance = (TLS_IO_INSTANCE*)handle;
            OPTIONHANDLER_HANDLE underlying_io_options;

            if ((underlying_io_options = xio_retrieveoptions(tls_io_instance->underlying_io)) == NULL ||
                OptionHandler_AddOption(result, OPTION_UNDERLYING_IO_OPTIONS, underlying_io_options) != OPTIONHANDLER_OK)
            {
                LogError("unable to save underlying_io options");
                OptionHandler_Destroy(underlying_io_options);
                OptionHandler_Destroy(result);
                result = NULL;
            }
            else if (
                (tls_io_instance->certificate != NULL) &&
                (OptionHandler_AddOption(result, OPTION_TRUSTED_CERT, tls_io_instance->certificate) != OPTIONHANDLER_OK)
                )
            {
                LogError("unable to save TrustedCerts option");
                OptionHandler_Destroy(result);
                result = NULL;
            }
            else if (
                (tls_io_instance->cipher_list != NULL) &&
                (OptionHandler_AddOption(result, OPTION_OPENSSL_CIPHER_SUITE, tls_io_instance->cipher_list) != OPTIONHANDLER_OK)
                )
            {
                LogError("unable to save CipherSuite option");
                OptionHandler_Destroy(result);
                result = NULL;
            }
            else if (tls_io_instance->x509_certificate != NULL && (OptionHandler_AddOption(result, SU_OPTION_X509_CERT, tls_io_instance->x509_certificate) != OPTIONHANDLER_OK) )
            {
                LogError("unable to save x509 certificate option");
                OptionHandler_Destroy(result);
                result = NULL;
            }
            else if (tls_io_instance->x509_private_key != NULL && (OptionHandler_AddOption(result, SU_OPTION_X509_PRIVATE_KEY, tls_io_instance->x509_private_key) != OPTIONHANDLER_OK) )
            {
                LogError("unable to save x509 privatekey option");
                OptionHandler_Destroy(result);
                result = NULL;
            }
            else if (tls_io_instance->tls_version != 0 && (OptionHandler_AddOption(result, OPTION_TLS_VERSION, &tls_io_instance->tls_version) != OPTIONHANDLER_OK) )
            {
                LogError("unable to save tls_version option");
                OptionHandler_Destroy(result);
                result = NULL;
            }
            else if (
                (tls_io_instance->engine_id != NULL) &&
                (OptionHandler_AddOption(result, OPTION_OPENSSL_ENGINE, tls_io_instance->engine_id) != OPTIONHANDLER_OK)
                )
            {
                LogError("unable to save Engine option");
                OptionHandler_Destroy(result);
                result = NULL;
            }
            else if (
                (tls_io_instance->x509_private_key_type != KEY_TYPE_DEFAULT) &&
                (OptionHandler_AddOption(result, OPTION_OPENSSL_PRIVATE_KEY_TYPE, &tls_io_instance->x509_private_key_type))
                )
            {
                LogError("unable to save x509PrivatekeyType option");
                OptionHandler_Destroy(result);
                result = NULL;
            }
            else if (tls_io_instance->tls_validation_callback != NULL)
            {
#ifdef WIN32
#pragma warning(push)
#pragma warning(disable:4152)
#endif
                void* ptr = tls_io_instance->tls_validation_callback;
#ifdef WIN32
#pragma warning(pop)
#endif
                if (OptionHandler_AddOption(result, "tls_validation_callback", (const char*)ptr) != OPTIONHANDLER_OK)
                {
                    LogError("unable to save tls_validation_callback option");
                    OptionHandler_Destroy(result);
                    result = NULL;
                }

                if (OptionHandler_AddOption(result, "tls_validation_callback_data", (const char*)tls_io_instance->tls_validation_callback_data) != OPTIONHANDLER_OK)
                {
                    LogError("unable to save tls_validation_callback_data option");
                    OptionHandler_Destroy(result);
                    result = NULL;
                }
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

static const IO_INTERFACE_DESCRIPTION tlsio_openssl_interface_description =
{
    tlsio_openssl_retrieveoptions,
    tlsio_openssl_create,
    tlsio_openssl_destroy,
    tlsio_openssl_open,
    tlsio_openssl_close,
    tlsio_openssl_send,
    tlsio_openssl_dowork,
    tlsio_openssl_setoption
};

static LOCK_HANDLE * openssl_locks = NULL;


static void openssl_lock_unlock_helper(LOCK_HANDLE lock, int lock_mode, const char* file, int line)
{
#ifdef NO_LOGGING
    // Avoid unused variable warning when logging not compiled in
    (void)file;
    (void)line;
#endif

    if (lock_mode & CRYPTO_LOCK)
    {
        if (Lock(lock) != LOCK_OK)
        {
            LogError("Failed to lock openssl lock (%s:%d)", file, line);
        }
    }
    else
    {
        if (Unlock(lock) != LOCK_OK)
        {
            LogError("Failed to unlock openssl lock (%s:%d)", file, line);
        }
    }
}

static void log_ERR_get_error(const char* message)
{
    char buf[128];
    AZURE_UNREFERENCED_PARAMETER(buf);
    unsigned long error;
    int i;

    if (message != NULL)
    {
        LogError("%s", message);
    }

    error = ERR_get_error();

    for (i = 0; 0 != error; i++)
    {
        LogError("  [%d] %s", i, ERR_error_string(error, buf));
        error = ERR_get_error();
    }
}

static STATIC_VAR_UNUSED struct CRYPTO_dynlock_value* openssl_dynamic_locks_create_cb(const char* file, int line)
{
#ifdef NO_LOGGING
    // Avoid unused variable warning when logging not compiled in
    (void)file;
    (void)line;
#endif

    struct CRYPTO_dynlock_value* result;

    result = malloc(sizeof(struct CRYPTO_dynlock_value));

    if (result == NULL)
    {
        LogError("Failed to allocate lock!  Out of memory (%s:%d).", file, line);
    }
    else
    {
        result->lock = Lock_Init();
        if (result->lock == NULL)
        {
            LogError("Failed to create lock for dynamic lock (%s:%d).", file, line);

            free(result);
            result = NULL;
        }
    }

    return result;
}

static STATIC_VAR_UNUSED void openssl_dynamic_locks_lock_unlock_cb(int lock_mode, struct CRYPTO_dynlock_value* dynlock_value, const char* file, int line)
{
    openssl_lock_unlock_helper(dynlock_value->lock, lock_mode, file, line);
}

static STATIC_VAR_UNUSED void openssl_dynamic_locks_destroy_cb(struct CRYPTO_dynlock_value* dynlock_value, const char* file, int line)
{
    (void)file;
    (void)line;
    Lock_Deinit(dynlock_value->lock);
    free(dynlock_value);
}

static void openssl_dynamic_locks_uninstall(void)
{
#if (OPENSSL_VERSION_NUMBER >= 0x00906000)
    CRYPTO_set_dynlock_create_callback(NULL);
    CRYPTO_set_dynlock_lock_callback(NULL);
    CRYPTO_set_dynlock_destroy_callback(NULL);
#endif
}

static void openssl_dynamic_locks_install(void)
{
#if (OPENSSL_VERSION_NUMBER >= 0x00906000)
    CRYPTO_set_dynlock_destroy_callback(openssl_dynamic_locks_destroy_cb);
    CRYPTO_set_dynlock_lock_callback(openssl_dynamic_locks_lock_unlock_cb);
    CRYPTO_set_dynlock_create_callback(openssl_dynamic_locks_create_cb);
#endif
}

static void STATIC_VAR_UNUSED openssl_static_locks_lock_unlock_cb(int lock_mode, int lock_index, const char * file, int line)
{
    if (lock_index < 0 || lock_index >= CRYPTO_num_locks())
    {
        LogError("Bad lock index %d passed (%s:%d)", lock_index, file, line);
    }
    else
    {
        openssl_lock_unlock_helper(openssl_locks[lock_index], lock_mode, file, line);
    }
}

static void openssl_static_locks_uninstall(void)
{
    if (openssl_locks != NULL)
    {
        int i;
        CRYPTO_set_locking_callback(NULL);

        for (i = 0; i < CRYPTO_num_locks(); i++)
        {
            if (openssl_locks[i] != NULL)
            {
                Lock_Deinit(openssl_locks[i]);
            }
        }

        free(openssl_locks);
        openssl_locks = NULL;
    }
    else
    {
        LogError("Locks already uninstalled");
    }
}

static int openssl_static_locks_install(void)
{
    int result;

    if (openssl_locks != NULL)
    {
        LogError("Locks already initialized");
        result = MU_FAILURE;
    }
    else
    {
        openssl_locks = malloc(CRYPTO_num_locks() * sizeof(LOCK_HANDLE));
        if (openssl_locks == NULL)
        {
            LogError("Failed to allocate locks");
            result = MU_FAILURE;
        }
        else
        {
            int i;
            for (i = 0; i < CRYPTO_num_locks(); i++)
            {
                openssl_locks[i] = Lock_Init();
                if (openssl_locks[i] == NULL)
                {
                    LogError("Failed to allocate lock %d", i);
                    break;
                }
            }

            if (i != CRYPTO_num_locks())
            {
                int j;
                for (j = 0; j < i; j++)
                {
                    Lock_Deinit(openssl_locks[j]);
                }
                result = MU_FAILURE;
            }
            else
            {
                CRYPTO_set_locking_callback(openssl_static_locks_lock_unlock_cb);

                result = 0;
            }
        }
    }
    return result;
}

static void indicate_error(TLS_IO_INSTANCE* tls_io_instance)
{
    if (tls_io_instance->on_io_error == NULL)
    {
        LogError("NULL on_io_error.");
    }
    else
    {
        tls_io_instance->on_io_error(tls_io_instance->on_io_error_context);
    }
}

static void indicate_open_complete(TLS_IO_INSTANCE* tls_io_instance, IO_OPEN_RESULT open_result)
{
    if (tls_io_instance->on_io_open_complete == NULL)
    {
        LogError("NULL on_io_open_complete.");
    }
    else
    {
        tls_io_instance->on_io_open_complete(tls_io_instance->on_io_open_complete_context, open_result);
    }
}

static int write_outgoing_bytes(TLS_IO_INSTANCE* tls_io_instance, ON_SEND_COMPLETE on_send_complete, void* callback_context)
{
    int result;

    size_t pending = BIO_ctrl_pending(tls_io_instance->out_bio);

    if (pending == 0)
    {
        result = 0;
    }
    else
    {
        unsigned char* bytes_to_send = malloc(pending);
        if (bytes_to_send == NULL)
        {
            LogError("NULL bytes_to_send.");
            result = MU_FAILURE;
        }
        else
        {
            if (BIO_read(tls_io_instance->out_bio, bytes_to_send, (int)pending) != (int)pending)
            {
                log_ERR_get_error("BIO_read not in pending state.");
                result = MU_FAILURE;
            }
            else
            {
                if (xio_send(tls_io_instance->underlying_io, bytes_to_send, pending, on_send_complete, callback_context) != 0)
                {
                    LogError("Error in xio_send.");
                    result = MU_FAILURE;
                }
                else
                {
                    result = 0;
                }
            }

            free(bytes_to_send);
        }
    }

    return result;
}

// Non-NULL tls_io_instance is guaranteed by callers.
// We are in TLSIO_STATE_IN_HANDSHAKE when entering this method.
static void send_handshake_bytes(TLS_IO_INSTANCE* tls_io_instance)
{
    int hsret;
    // ERR_clear_error must be called before any call that might set an
    // SSL_get_error result
    ERR_clear_error();
    hsret = SSL_do_handshake(tls_io_instance->ssl);
    if (hsret != SSL_DO_HANDSHAKE_SUCCESS)
    {
        int ssl_err = SSL_get_error(tls_io_instance->ssl, hsret);
        if (ssl_err != SSL_ERROR_WANT_READ && ssl_err != SSL_ERROR_WANT_WRITE)
        {
            if (ssl_err == SSL_ERROR_SSL)
            {
                LogError("%s", ERR_error_string(ERR_get_error(), NULL));
            }
            else
            {
                LogError("SSL handshake failed: %d", ssl_err);
            }
            tls_io_instance->tlsio_state = TLSIO_STATE_HANDSHAKE_FAILED;
        }
        else
        {
            if (write_outgoing_bytes(tls_io_instance, NULL, NULL) != 0)
            {
                LogError("Error in write_outgoing_bytes.");
                tls_io_instance->tlsio_state = TLSIO_STATE_HANDSHAKE_FAILED;
            }
        }
    }
    else
    {
        tls_io_instance->tlsio_state = TLSIO_STATE_OPEN;
        indicate_open_complete(tls_io_instance, IO_OPEN_OK);
    }
}

void engine_destroy(TLS_IO_INSTANCE* tls)
{
    if(tls->engine != NULL)
    {
        ENGINE_free(tls->engine); // Release structural reference.
        tls->engine = NULL;
    }
}

int engine_load(TLS_IO_INSTANCE* tls)
{
    int result;

    // TLSIO is only responsible for the Engine structural reference.
    tls->engine = ENGINE_by_id(tls->engine_id);
    if (tls->engine == NULL)
    {
        log_ERR_get_error("unable to load engine by ID.");
        result = MU_FAILURE;
    }
    else
    {
        result = 0;
    }

    return result;
}

static void close_openssl_instance(TLS_IO_INSTANCE* tls_io_instance)
{
    if (tls_io_instance->ssl != NULL)
    {
        SSL_free(tls_io_instance->ssl);
        tls_io_instance->ssl = NULL;
    }
    if (tls_io_instance->ssl_context != NULL)
    {
        SSL_CTX_free(tls_io_instance->ssl_context);
        tls_io_instance->ssl_context = NULL;
    }
}

static void on_underlying_io_close_complete(void* context)
{
    TLS_IO_INSTANCE* tls_io_instance = (TLS_IO_INSTANCE*)context;

    switch (tls_io_instance->tlsio_state)
    {
    default:
    case TLSIO_STATE_NOT_OPEN:
    case TLSIO_STATE_OPEN:
    case TLSIO_STATE_IN_HANDSHAKE:
    case TLSIO_STATE_HANDSHAKE_FAILED:
    case TLSIO_STATE_ERROR:
        break;

    case TLSIO_STATE_OPENING_UNDERLYING_IO:
        tls_io_instance->tlsio_state = TLSIO_STATE_NOT_OPEN;
        indicate_open_complete(tls_io_instance, IO_OPEN_ERROR);
        break;

    case TLSIO_STATE_CLOSING:
        tls_io_instance->tlsio_state = TLSIO_STATE_NOT_OPEN;

        if (tls_io_instance->on_io_close_complete != NULL)
        {
            tls_io_instance->on_io_close_complete(tls_io_instance->on_io_close_complete_context);
        }
        break;
    }

    close_openssl_instance(tls_io_instance);
}

static void on_underlying_io_open_complete(void* context, IO_OPEN_RESULT open_result)
{
    TLS_IO_INSTANCE* tls_io_instance = (TLS_IO_INSTANCE*)context;

    if (tls_io_instance->tlsio_state == TLSIO_STATE_OPENING_UNDERLYING_IO)
    {
        if (open_result == IO_OPEN_OK)
        {
            tls_io_instance->tlsio_state = TLSIO_STATE_IN_HANDSHAKE;

            // Begin the handshake process here. It continues in on_underlying_io_bytes_received
            send_handshake_bytes(tls_io_instance);
        }
        else
        {
            tls_io_instance->tlsio_state = TLSIO_STATE_NOT_OPEN;
            indicate_open_complete(tls_io_instance, IO_OPEN_ERROR);
            LogError("Invalid tlsio_state. Expected state is TLSIO_STATE_OPENING_UNDERLYING_IO.");
        }
    }
}

static void on_underlying_io_error(void* context)
{
    TLS_IO_INSTANCE* tls_io_instance = (TLS_IO_INSTANCE*)context;

    switch (tls_io_instance->tlsio_state)
    {
    default:
        break;

    case TLSIO_STATE_OPENING_UNDERLYING_IO:
    case TLSIO_STATE_IN_HANDSHAKE:
        tls_io_instance->tlsio_state = TLSIO_STATE_NOT_OPEN;
        indicate_open_complete(tls_io_instance, IO_OPEN_ERROR);
        break;

    case TLSIO_STATE_OPEN:
        indicate_error(tls_io_instance);
        break;
    }
}

static int decode_ssl_received_bytes(TLS_IO_INSTANCE* tls_io_instance)
{
    int result = 0;
    unsigned char buffer[64];

    int rcv_bytes = 1;

    while (rcv_bytes > 0)
    {
        if (tls_io_instance->ssl == NULL)
        {
            LogError("SSL channel closed in decode_ssl_received_bytes.");
            result = MU_FAILURE;
            return result;
        }

        rcv_bytes = SSL_read(tls_io_instance->ssl, buffer, sizeof(buffer));
        if (rcv_bytes > 0)
        {
            if (tls_io_instance->on_bytes_received == NULL)
            {
                LogError("NULL on_bytes_received.");
            }
            else
            {
                tls_io_instance->on_bytes_received(tls_io_instance->on_bytes_received_context, buffer, rcv_bytes);
            }
        }
    }

    return result;
}

static void on_underlying_io_bytes_received(void* context, const unsigned char* buffer, size_t size)
{
    TLS_IO_INSTANCE* tls_io_instance = (TLS_IO_INSTANCE*)context;

    int written = BIO_write(tls_io_instance->in_bio, buffer, (int)size);
    if (written != (int)size)
    {
        tls_io_instance->tlsio_state = TLSIO_STATE_ERROR;
        indicate_error(tls_io_instance);
        log_ERR_get_error("Error in BIO_write.");
    }
    else
    {
        switch (tls_io_instance->tlsio_state)
        {
        default:
            break;

        case TLSIO_STATE_IN_HANDSHAKE:
            send_handshake_bytes(tls_io_instance);
            break;

        case TLSIO_STATE_OPEN:
            if (decode_ssl_received_bytes(tls_io_instance) != 0)
            {
                tls_io_instance->tlsio_state = TLSIO_STATE_ERROR;
                indicate_error(tls_io_instance);
                LogError("Error in decode_ssl_received_bytes.");
            }
            break;
        }
    }
}

static int add_certificate_to_store(TLS_IO_INSTANCE* tls_io_instance, const char* certValue)
{
    int result = 0;

    if (certValue != NULL)
    {
        X509_STORE* cert_store = SSL_CTX_get_cert_store(tls_io_instance->ssl_context);
        if (cert_store == NULL)
        {
            log_ERR_get_error("failure in SSL_CTX_get_cert_store.");
            result = MU_FAILURE;
        }
        else
        {
#if (OPENSSL_VERSION_NUMBER >= 0x10100000L) && (OPENSSL_VERSION_NUMBER < 0x20000000L)
            const BIO_METHOD* bio_method;
#else
            BIO_METHOD* bio_method;
#endif
            bio_method = BIO_s_mem();
            if (bio_method == NULL)
            {
                log_ERR_get_error("failure in BIO_s_mem");
                result = MU_FAILURE;
            }
            else
            {
                BIO* cert_memory_bio = BIO_new(bio_method);

                if (cert_memory_bio == NULL)
                {
                    log_ERR_get_error("failure in BIO_new");
                    result = MU_FAILURE;
                }
                else
                {
                    int puts_result = BIO_puts(cert_memory_bio, certValue);
                    if (puts_result < 0)
                    {
                        log_ERR_get_error("failure in BIO_puts");
                        result = MU_FAILURE;
                    }
                    else
                    {
                        if ((size_t)puts_result != strlen(certValue))
                        {
                            log_ERR_get_error("mismatching legths");
                            result = MU_FAILURE;
                        }
                        else
                        {
                            X509* certificate;
                            while ((certificate = PEM_read_bio_X509(cert_memory_bio, NULL, NULL, NULL)) != NULL)
                            {
                                if (!X509_STORE_add_cert(cert_store, certificate))
                                {
                                    X509_free(certificate);
                                    log_ERR_get_error("failure in X509_STORE_add_cert");
                                    break;
                                }
                                X509_free(certificate);
                            }
                            if (certificate == NULL)
                            {
                                result = 0;/*all is fine*/
                            }
                            else
                            {
                                /*previous while loop terminated unfortunately*/
                                result = MU_FAILURE;
                            }
                        }
                    }
                    BIO_free(cert_memory_bio);
                }
            }
        }
    }
    return result;
}

static int enable_domain_check(TLS_IO_INSTANCE* tlsInstance)
{
    int result = 0;

    if (!tlsInstance->ignore_host_name_check)
    {
#if (OPENSSL_VERSION_NUMBER < 0x10002000L)
#error "OpenSSL v1.0.2 or above required. See https://wiki.openssl.org/index.php/Hostname_validation for details."
#endif
        X509_VERIFY_PARAM *param = SSL_get0_param(tlsInstance->ssl);

        X509_VERIFY_PARAM_set_hostflags(param, 0);
        if (!X509_VERIFY_PARAM_set1_host(param, tlsInstance->hostname, strlen(tlsInstance->hostname)))
        {
            result = MU_FAILURE;
        }
        else
        {
            SSL_set_verify(tlsInstance->ssl, SSL_VERIFY_PEER, NULL);
        }
    }

    return result;
}

static int create_openssl_instance(TLS_IO_INSTANCE* tlsInstance)
{
    int result;

    const SSL_METHOD* method = NULL;

#if (OPENSSL_VERSION_NUMBER < 0x10100000L) || (OPENSSL_VERSION_NUMBER >= 0x20000000L)
    if (tlsInstance->tls_version == VERSION_1_2)
    {
        method = TLSv1_2_method();
    }
    else if (tlsInstance->tls_version == VERSION_1_1)
    {
        method = TLSv1_1_method();
    }
    else
    {
        method = TLSv1_method();
    }
#else
    {
        method = TLS_method();
    }
#endif

    tlsInstance->ssl_context = SSL_CTX_new(method);
    if (tlsInstance->ssl_context == NULL)
    {
        log_ERR_get_error("Failed allocating OpenSSL context.");
        result = MU_FAILURE;
    }
    else if ((tlsInstance->engine_id != NULL) &&
             (engine_load(tlsInstance) != 0))
    {
        SSL_CTX_free(tlsInstance->ssl_context);
        tlsInstance->ssl_context = NULL;
        result = MU_FAILURE;
    }
    else if ((tlsInstance->cipher_list != NULL) &&
             (SSL_CTX_set_cipher_list(tlsInstance->ssl_context, tlsInstance->cipher_list)) != 1)
    {
        engine_destroy(tlsInstance);
        SSL_CTX_free(tlsInstance->ssl_context);
        tlsInstance->ssl_context = NULL;
        log_ERR_get_error("unable to set cipher list.");
        result = MU_FAILURE;
    }
    else if (add_certificate_to_store(tlsInstance, tlsInstance->certificate) != 0)
    {
        engine_destroy(tlsInstance);
        SSL_CTX_free(tlsInstance->ssl_context);
        tlsInstance->ssl_context = NULL;
        log_ERR_get_error("unable to add_certificate_to_store.");
        result = MU_FAILURE;
    }
    /*x509 authentication can only be build before underlying connection is realized*/
    else if (
        (tlsInstance->x509_certificate != NULL) &&
        (tlsInstance->x509_private_key != NULL) &&
        (x509_openssl_add_credentials(
            tlsInstance->ssl_context, 
            tlsInstance->x509_certificate, 
            tlsInstance->x509_private_key,
            tlsInstance->x509_private_key_type,
            tlsInstance->engine) != 0)
        )
    {
        engine_destroy(tlsInstance);
        SSL_CTX_free(tlsInstance->ssl_context);
        tlsInstance->ssl_context = NULL;
        log_ERR_get_error("unable to use x509 authentication");
        result = MU_FAILURE;
    }
    else
    {
        SSL_CTX_set_cert_verify_callback(tlsInstance->ssl_context, tlsInstance->tls_validation_callback, tlsInstance->tls_validation_callback_data);

        tlsInstance->in_bio = BIO_new(BIO_s_mem());
        if (tlsInstance->in_bio == NULL)
        {
            engine_destroy(tlsInstance);
            SSL_CTX_free(tlsInstance->ssl_context);
            tlsInstance->ssl_context = NULL;
            log_ERR_get_error("Failed BIO_new for in BIO.");
            result = MU_FAILURE;
        }
        else
        {
            tlsInstance->out_bio = BIO_new(BIO_s_mem());
            if (tlsInstance->out_bio == NULL)
            {
                (void)BIO_free(tlsInstance->in_bio);
                engine_destroy(tlsInstance);
                SSL_CTX_free(tlsInstance->ssl_context);
                tlsInstance->ssl_context = NULL;
                log_ERR_get_error("Failed BIO_new for out BIO.");
                result = MU_FAILURE;
            }
            else
            {
                if ((BIO_set_mem_eof_return(tlsInstance->in_bio, -1) <= 0) ||
                    (BIO_set_mem_eof_return(tlsInstance->out_bio, -1) <= 0))
                {
                    (void)BIO_free(tlsInstance->in_bio);
                    (void)BIO_free(tlsInstance->out_bio);
                    engine_destroy(tlsInstance);
                    SSL_CTX_free(tlsInstance->ssl_context);
                    tlsInstance->ssl_context = NULL;
                    LogError("Failed BIO_set_mem_eof_return.");
                    result = MU_FAILURE;
                }
                else
                {
                    SSL_CTX_set_verify(tlsInstance->ssl_context, SSL_VERIFY_PEER, NULL);

                    // Specifies that the default locations for which CA certificates are loaded should be used.
                    if (SSL_CTX_set_default_verify_paths(tlsInstance->ssl_context) != 1)
                    {
                        // This is only a warning to the user. They can still specify the certificate via SetOption.
                        LogInfo("WARNING: Unable to specify the default location for CA certificates on this platform.");
                    }

                    tlsInstance->ssl = SSL_new(tlsInstance->ssl_context);

                    if (tlsInstance->ssl == NULL)
                    {
                        (void)BIO_free(tlsInstance->in_bio);
                        (void)BIO_free(tlsInstance->out_bio);
                        engine_destroy(tlsInstance);
                        SSL_CTX_free(tlsInstance->ssl_context);
                        tlsInstance->ssl_context = NULL;
                        log_ERR_get_error("Failed creating OpenSSL instance.");
                        result = MU_FAILURE;
                    }
                    else if (SSL_set_tlsext_host_name(tlsInstance->ssl, tlsInstance->hostname) != 1)
                    {
                        SSL_free(tlsInstance->ssl);
                        tlsInstance->ssl = NULL;
                        (void)BIO_free(tlsInstance->in_bio);
                        (void)BIO_free(tlsInstance->out_bio);
                        engine_destroy(tlsInstance);
                        SSL_CTX_free(tlsInstance->ssl_context);
                        tlsInstance->ssl_context = NULL;
                        log_ERR_get_error("Failed setting SNI hostname hint.");
                        result = MU_FAILURE;
                    }
                    else if (enable_domain_check(tlsInstance))
                    {
                        SSL_free(tlsInstance->ssl);
                        tlsInstance->ssl = NULL;
                        (void)BIO_free(tlsInstance->in_bio);
                        (void)BIO_free(tlsInstance->out_bio);
                        SSL_CTX_free(tlsInstance->ssl_context);
                        tlsInstance->ssl_context = NULL;
                        log_ERR_get_error("Failed to configure domain name verification.");
                        result = MU_FAILURE;
                    }
                    else
                    {
                        SSL_set_bio(tlsInstance->ssl, tlsInstance->in_bio, tlsInstance->out_bio);
                        SSL_set_connect_state(tlsInstance->ssl);
                        result = 0;
                    }
                }
            }
        }
    }
    return result;
}

int tlsio_openssl_init(void)
{
    (void)SSL_library_init();

    SSL_load_error_strings();
    ERR_load_BIO_strings();
    OpenSSL_add_all_algorithms();

    if (openssl_static_locks_install() != 0)
    {
        LogError("Failed to install static locks in OpenSSL!");
        return MU_FAILURE;
    }

    openssl_dynamic_locks_install();
    return 0;
}

void tlsio_openssl_deinit(void)
{
    openssl_dynamic_locks_uninstall();
    openssl_static_locks_uninstall();
#if  (OPENSSL_VERSION_NUMBER >= 0x00907000L) &&  (OPENSSL_VERSION_NUMBER < 0x20000000L) && (FIPS_mode_set)
    FIPS_mode_set(0);
#endif
    CRYPTO_set_locking_callback(NULL);
    CRYPTO_set_id_callback(NULL);
    ERR_free_strings();
    EVP_cleanup();

#if   (OPENSSL_VERSION_NUMBER < 0x10000000L)
    ERR_remove_state(0);
#elif (OPENSSL_VERSION_NUMBER < 0x10100000L) || (OPENSSL_VERSION_NUMBER >= 0x20000000L)
    ERR_remove_thread_state(NULL);
#endif
#if  (OPENSSL_VERSION_NUMBER >= 0x10002000L) &&  (OPENSSL_VERSION_NUMBER < 0x10010000L) && (SSL_COMP_free_compression_methods)
    SSL_COMP_free_compression_methods();
#endif
    CRYPTO_cleanup_all_ex_data();
}

CONCRETE_IO_HANDLE tlsio_openssl_create(void* io_create_parameters)
{
    TLSIO_CONFIG* tls_io_config = io_create_parameters;
    TLS_IO_INSTANCE* result;

    if (tls_io_config == NULL)
    {
        result = NULL;
        LogError("NULL tls_io_config.");
    }
    else
    {
        result = malloc(sizeof(TLS_IO_INSTANCE));
        if (result == NULL)
        {
            LogError("Failed allocating TLSIO instance.");
        }
        else if (mallocAndStrcpy_s(&result->hostname, tls_io_config->hostname) != 0)
        {
            LogError("Failed copying the target hostname.");
            free(result);
            result = NULL;
        }
        else
        {
            SOCKETIO_CONFIG socketio_config;
            const IO_INTERFACE_DESCRIPTION* underlying_io_interface;
            void* io_interface_parameters;

            if (tls_io_config->underlying_io_interface != NULL)
            {
                underlying_io_interface = tls_io_config->underlying_io_interface;
                io_interface_parameters = tls_io_config->underlying_io_parameters;
            }
            else
            {
                socketio_config.hostname = tls_io_config->hostname;
                socketio_config.port = tls_io_config->port;
                socketio_config.accepted_socket = NULL;

                underlying_io_interface = socketio_get_interface_description();
                io_interface_parameters = &socketio_config;
            }

            if (underlying_io_interface == NULL)
            {
                free(result->hostname);
                free(result);
                result = NULL;
                LogError("Failed getting socket IO interface description.");
            }
            else
            {
                result->certificate = NULL;
                result->cipher_list = NULL;
                result->in_bio = NULL;
                result->out_bio = NULL;
                result->on_bytes_received = NULL;
                result->on_bytes_received_context = NULL;
                result->on_io_open_complete = NULL;
                result->on_io_open_complete_context = NULL;
                result->on_io_close_complete = NULL;
                result->on_io_close_complete_context = NULL;
                result->on_io_error = NULL;
                result->on_io_error_context = NULL;
                result->ssl = NULL;
                result->ssl_context = NULL;
                result->tls_validation_callback = NULL;
                result->tls_validation_callback_data = NULL;
                result->x509_certificate = NULL;
                result->x509_private_key = NULL;
                result->ignore_host_name_check = false;
                result->engine_id = NULL;
                result->engine = NULL;
                result->x509_private_key_type = KEY_TYPE_DEFAULT;

                result->tls_version = VERSION_1_2;

                result->underlying_io = xio_create(underlying_io_interface, io_interface_parameters);
                if (result->underlying_io == NULL)
                {
                    free(result->hostname);
                    free(result);
                    result = NULL;
                    LogError("Failed xio_create.");
                }
                else
                {
                    result->tlsio_state = TLSIO_STATE_NOT_OPEN;
                }
            }
        }
    }

    return result;
}

void tlsio_openssl_destroy(CONCRETE_IO_HANDLE tls_io)
{
    if (tls_io == NULL)
    {
        LogError("NULL tls_io.");
    }
    else
    {
        TLS_IO_INSTANCE* tls_io_instance = (TLS_IO_INSTANCE*)tls_io;
        engine_destroy(tls_io_instance);

        if (tls_io_instance->certificate != NULL)
        {
            free(tls_io_instance->certificate);
            tls_io_instance->certificate = NULL;
        }
        if (tls_io_instance->cipher_list != NULL)
        {
            free(tls_io_instance->cipher_list);
            tls_io_instance->cipher_list = NULL;
        }
        free((void*)tls_io_instance->x509_certificate);
        free((void*)tls_io_instance->x509_private_key);
        close_openssl_instance(tls_io_instance);
        if (tls_io_instance->underlying_io != NULL)
        {
            xio_destroy(tls_io_instance->underlying_io);
            tls_io_instance->underlying_io = NULL;
        }
        free(tls_io_instance->hostname);
        if (tls_io_instance->engine_id != NULL)
        {
            free(tls_io_instance->engine_id);
            tls_io_instance->engine_id = NULL;
        }

        free(tls_io);
    }
}

int tlsio_openssl_open(CONCRETE_IO_HANDLE tls_io, ON_IO_OPEN_COMPLETE on_io_open_complete, void* on_io_open_complete_context, ON_BYTES_RECEIVED on_bytes_received, void* on_bytes_received_context, ON_IO_ERROR on_io_error, void* on_io_error_context)
{
    int result;

    if (tls_io == NULL)
    {
        result = MU_FAILURE;
        LogError("NULL tls_io.");
    }
    else
    {
        TLS_IO_INSTANCE* tls_io_instance = (TLS_IO_INSTANCE*)tls_io;

        if (tls_io_instance->tlsio_state != TLSIO_STATE_NOT_OPEN)
        {
            LogError("Invalid tlsio_state. Expected state is TLSIO_STATE_NOT_OPEN.");
            result = MU_FAILURE;
        }
        else
        {
            tls_io_instance->on_io_open_complete = on_io_open_complete;
            tls_io_instance->on_io_open_complete_context = on_io_open_complete_context;

            tls_io_instance->on_bytes_received = on_bytes_received;
            tls_io_instance->on_bytes_received_context = on_bytes_received_context;

            tls_io_instance->on_io_error = on_io_error;
            tls_io_instance->on_io_error_context = on_io_error_context;

            tls_io_instance->tlsio_state = TLSIO_STATE_OPENING_UNDERLYING_IO;

            if (create_openssl_instance(tls_io_instance) != 0)
            {
                LogError("Failed creating the OpenSSL instance.");
                tls_io_instance->tlsio_state = TLSIO_STATE_NOT_OPEN;
                result = MU_FAILURE;
            }
            else if (xio_open(tls_io_instance->underlying_io, on_underlying_io_open_complete, tls_io_instance,
                on_underlying_io_bytes_received, tls_io_instance, on_underlying_io_error, tls_io_instance) != 0)
            {
                LogError("Failed opening the underlying I/O.");
                close_openssl_instance(tls_io_instance);
                tls_io_instance->tlsio_state = TLSIO_STATE_NOT_OPEN;
                result = MU_FAILURE;
            }
            else
            {
                result = 0;
            }
        }
    }

    return result;
}

/* Codes_SRS_TLSIO_30_009: [ The phrase "enter TLSIO_STATE_EXT_CLOSING" means the adapter shall iterate through any unsent messages in the queue and shall delete each message after calling its on_send_complete with the associated callback_context and IO_SEND_CANCELLED. ]*/
/* Codes_SRS_TLSIO_30_006: [ The phrase "enter TLSIO_STATE_EXT_CLOSED" means the adapter shall forcibly close any existing connections then call the on_io_close_complete function and pass the on_io_close_complete_context that was supplied in tlsio_close_async. ]*/
/* Codes_SRS_TLSIO_30_051: [ On success, if the underlying TLS does not support asynchronous closing, then the adapter shall enter TLSIO_STATE_EXT_CLOSED immediately after entering TLSIO_STATE_EX_CLOSING. ]*/
int tlsio_openssl_close(CONCRETE_IO_HANDLE tls_io, ON_IO_CLOSE_COMPLETE on_io_close_complete, void* callback_context)
{
    int result;

    /* Codes_SRS_TLSIO_30_050: [ If the tlsio_handle parameter is NULL, tlsio_close_async shall log an error and return _FAILURE_. ]*/
    if (tls_io == NULL)
    {
        LogError("NULL tls_io.");
        result = MU_FAILURE;
    }
    else
    {
        TLS_IO_INSTANCE* tls_io_instance = (TLS_IO_INSTANCE*)tls_io;

        if (tls_io_instance->tlsio_state != TLSIO_STATE_ERROR && tls_io_instance->tlsio_state != TLSIO_STATE_OPEN)
        {
            /* Codes_RS_TLSIO_30_053: [ If the adapter is in any state other than TLSIO_STATE_EXT_OPEN or TLSIO_STATE_EXT_ERROR then tlsio_close_async shall log that tlsio_close_async has been called and then continue normally. ]*/
            // LogInfo rather than LogError because this is an unusual but not erroneous situation
            LogInfo("Closing tlsio from a state other than TLSIO_STATE_EXT_OPEN or TLSIO_STATE_EXT_ERROR");
        }

        if (is_an_opening_state(tls_io_instance->tlsio_state))
        {
            /* Codes_SRS_TLSIO_30_057: [ On success, if the adapter is in TLSIO_STATE_EXT_OPENING, it shall call on_io_open_complete with the on_io_open_complete_context supplied in tlsio_open_async and IO_OPEN_CANCELLED. This callback shall be made before changing the internal state of the adapter. ]*/
            tls_io_instance->on_io_open_complete(tls_io_instance->on_io_open_complete_context, IO_OPEN_CANCELLED);
        }

        if (tls_io_instance->tlsio_state == TLSIO_STATE_OPEN)
        {
            // Attempt a graceful shutdown
            /* Codes_SRS_TLSIO_30_056: [ On success the adapter shall enter TLSIO_STATE_EX_CLOSING. ]*/
            tls_io_instance->tlsio_state = TLSIO_STATE_CLOSING;
            tls_io_instance->on_io_close_complete = on_io_close_complete;
            tls_io_instance->on_io_close_complete_context = callback_context;
            // xio_close is guaranteed to succeed from the open state, and the callback completes the
            // transition into TLSIO_STATE_NOT_OPEN
            if (xio_close(tls_io_instance->underlying_io, on_underlying_io_close_complete, tls_io_instance) != 0)
            {
                close_openssl_instance(tls_io_instance);
                tls_io_instance->tlsio_state = TLSIO_STATE_NOT_OPEN;
            }
        }
        else
        {
            // Just force the shutdown
            /* Codes_SRS_TLSIO_30_056: [ On success the adapter shall enter TLSIO_STATE_EX_CLOSING. ]*/
            /* Codes_SRS_TLSIO_30_051: [ On success, if the underlying TLS does not support asynchronous closing or if the adapter is not in TLSIO_STATE_EXT_OPEN, then the adapter shall enter TLSIO_STATE_EXT_CLOSED immediately after entering TLSIO_STATE_EXT_CLOSING. ]*/
            // Current implementations of xio_close will fail if not in the open state, but we don't care
            (void)xio_close(tls_io_instance->underlying_io, NULL, NULL);
            close_openssl_instance(tls_io_instance);
            tls_io_instance->tlsio_state = TLSIO_STATE_NOT_OPEN;
        }

        result = 0;
    }
    /* Codes_SRS_TLSIO_30_054: [ On failure, the adapter shall not call on_io_close_complete. ]*/

    return result;
}

int tlsio_openssl_send(CONCRETE_IO_HANDLE tls_io, const void* buffer, size_t size, ON_SEND_COMPLETE on_send_complete, void* callback_context)
{
    int result;

    if (tls_io == NULL)
    {
        LogError("NULL tls_io.");
        result = MU_FAILURE;
    }
    else
    {
        TLS_IO_INSTANCE* tls_io_instance = (TLS_IO_INSTANCE*)tls_io;

        if (tls_io_instance->tlsio_state != TLSIO_STATE_OPEN)
        {
            LogError("Invalid tlsio_state. Expected state is TLSIO_STATE_OPEN.");
            result = MU_FAILURE;
        }
        else
        {
            int res;
            if (tls_io_instance->ssl == NULL)
            {
                LogError("SSL channel closed in tlsio_openssl_send.");
                result = MU_FAILURE;
                return result;
            }

            res = SSL_write(tls_io_instance->ssl, buffer, (int)size);
            if (res != (int)size)
            {
                log_ERR_get_error("SSL_write error.");
                result = MU_FAILURE;
            }
            else
            {
                if (write_outgoing_bytes(tls_io_instance, on_send_complete, callback_context) != 0)
                {
                    LogError("Error in write_outgoing_bytes.");
                    result = MU_FAILURE;
                }
                else
                {
                    result = 0;
                }
            }
        }
    }

    return result;
}

void tlsio_openssl_dowork(CONCRETE_IO_HANDLE tls_io)
{
    if (tls_io == NULL)
    {
        LogError("NULL tls_io.");
    }
    else
    {
        TLS_IO_INSTANCE* tls_io_instance = (TLS_IO_INSTANCE*)tls_io;

        switch (tls_io_instance->tlsio_state)
        {
        case TLSIO_STATE_OPENING_UNDERLYING_IO:
        case TLSIO_STATE_IN_HANDSHAKE:
        case TLSIO_STATE_OPEN:
            /* this is needed in order to pump out bytes produces by OpenSSL for things like renegotiation */
            write_outgoing_bytes(tls_io_instance, NULL, NULL);
            break;
        case TLSIO_STATE_NOT_OPEN:
        case TLSIO_STATE_HANDSHAKE_FAILED:
        case TLSIO_STATE_CLOSING:
        case TLSIO_STATE_ERROR:
        default:
            break;
        }

        if (tls_io_instance->tlsio_state != TLSIO_STATE_NOT_OPEN)
        {
            /* Same behavior as schannel */
            xio_dowork(tls_io_instance->underlying_io);

            if (tls_io_instance->tlsio_state == TLSIO_STATE_HANDSHAKE_FAILED)
            {
                // The handshake failed so we need to close. The tlsio becomes aware of the
                // handshake failure during an on_bytes_received while the underlying
                // xio_dowork is pumping data out of the socket in a while loop. The tlsio can't
                // close down during the callback because that would mean the xio_dowork would
                // be trying to read from a closed socket. So instead, the tlsio sets its state
                // to TLSIO_STATE_HANDSHAKE_FAILED during the on_bytes_received callback,
                // can then gracefully shut things down here.
                //
                // Set the state to TLSIO_STATE_ERROR so close won't gripe about the state
                tls_io_instance->tlsio_state = TLSIO_STATE_ERROR;
                tlsio_openssl_close(tls_io_instance, NULL, NULL);
                indicate_open_complete(tls_io_instance, IO_OPEN_ERROR);
            }
        }
    }
}

int tlsio_openssl_setoption(CONCRETE_IO_HANDLE tls_io, const char* optionName, const void* value)
{
    int result;

    if (tls_io == NULL || optionName == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        TLS_IO_INSTANCE* tls_io_instance = (TLS_IO_INSTANCE*)tls_io;

        if (strcmp(OPTION_TRUSTED_CERT, optionName) == 0)
        {
            const char* cert = (const char*)value;
            size_t len;

            if (tls_io_instance->certificate != NULL)
            {
                // Free the memory if it has been previously allocated
                free(tls_io_instance->certificate);
                tls_io_instance->certificate = NULL;
            }

            // Store the certificate
            len = strlen(cert);
            tls_io_instance->certificate = malloc(len + 1);
            if (tls_io_instance->certificate == NULL)
            {
                result = MU_FAILURE;
            }
            else
            {
                strcpy(tls_io_instance->certificate, cert);
                result = 0;
            }

            // If we're previously connected then add the cert to the context
            if (tls_io_instance->ssl_context != NULL)
            {
                result = add_certificate_to_store(tls_io_instance, cert);
            }
        }
        else if (strcmp(OPTION_OPENSSL_CIPHER_SUITE, optionName) == 0)
        {
            if (tls_io_instance->cipher_list != NULL)
            {
                // Free the memory if it has been previously allocated
                free(tls_io_instance->cipher_list);
                tls_io_instance->cipher_list = NULL;
            }

            // Store the cipher suites
            if (mallocAndStrcpy_s((char**)&tls_io_instance->cipher_list, value) != 0)
            {
                LogError("unable to mallocAndStrcpy_s %s", optionName);
                result = MU_FAILURE;
            }
            else
            {
                result = 0;
            }
        }
        else if (strcmp(SU_OPTION_X509_CERT, optionName) == 0 || strcmp(OPTION_X509_ECC_CERT, optionName) == 0)
        {
            if (tls_io_instance->x509_certificate != NULL)
            {
                LogError("unable to set x509 options more than once");
                result = MU_FAILURE;
            }
            else
            {
                /*let's make a copy of this option*/
                if (mallocAndStrcpy_s((char**)&tls_io_instance->x509_certificate, value) != 0)
                {
                    LogError("unable to mallocAndStrcpy_s %s", optionName);
                    result = MU_FAILURE;
                }
                else
                {
                    result = 0;
                }
            }
        }
        else if (strcmp(SU_OPTION_X509_PRIVATE_KEY, optionName) == 0 || strcmp(OPTION_X509_ECC_KEY, optionName) == 0)
        {
            if (tls_io_instance->x509_private_key != NULL)
            {
                LogError("unable to set more than once x509 options");
                result = MU_FAILURE;
            }
            else
            {
                /*let's make a copy of this option*/
                if (mallocAndStrcpy_s((char**)&tls_io_instance->x509_private_key, value) != 0)
                {
                    LogError("unable to mallocAndStrcpy_s %s", optionName);
                    result = MU_FAILURE;
                }
                else
                {
                    result = 0;
                }
            }
        }
        else if (strcmp(OPTION_OPENSSL_ENGINE, optionName) == 0)
        {
            ENGINE_load_builtin_engines();

            if (mallocAndStrcpy_s((char**)&tls_io_instance->engine_id, value) != 0)
            {
                LogError("unable to mallocAndStrcpy_s %s", optionName);
                result = MU_FAILURE;
            }
            else
            {
                result = 0;
            }
        }
        else if (strcmp(OPTION_OPENSSL_PRIVATE_KEY_TYPE, optionName) == 0)
        {
            const OPTION_OPENSSL_KEY_TYPE type = *(const OPTION_OPENSSL_KEY_TYPE*)value;
            switch (type)
            {
            case KEY_TYPE_DEFAULT:
            case KEY_TYPE_ENGINE:
                tls_io_instance->x509_private_key_type = type;
                result = 0;
                break;

            default:
                LogError("Unknown x509PrivatekeyType type %d", type);
                result = MU_FAILURE;
            }
        }
        else if (strcmp("tls_validation_callback", optionName) == 0)
        {
#ifdef WIN32
#pragma warning(push)
#pragma warning(disable:4055)
#endif // WIN32
            tls_io_instance->tls_validation_callback = (TLS_CERTIFICATE_VALIDATION_CALLBACK)value;
#ifdef WIN32
#pragma warning(pop)
#endif // WIN32

            if (tls_io_instance->ssl_context != NULL)
            {
                SSL_CTX_set_cert_verify_callback(tls_io_instance->ssl_context, tls_io_instance->tls_validation_callback, tls_io_instance->tls_validation_callback_data);
            }

            result = 0;
        }
        else if (strcmp("tls_validation_callback_data", optionName) == 0)
        {
            tls_io_instance->tls_validation_callback_data = (void*)value;

            if (tls_io_instance->ssl_context != NULL)
            {
                SSL_CTX_set_cert_verify_callback(tls_io_instance->ssl_context, tls_io_instance->tls_validation_callback, tls_io_instance->tls_validation_callback_data);
            }

            result = 0;
        }
        else if (strcmp(OPTION_TLS_VERSION, optionName) == 0)
        {
            if (tls_io_instance->ssl_context != NULL)
            {
                LogError("Unable to set the tls version after the tls connection is established");
                result = MU_FAILURE;
            }
            else
            {
                const int version_option = *(const int*)value;
                if (version_option == 0 || version_option == 10)
                {
                    tls_io_instance->tls_version = VERSION_1_0;
                }
                else if (version_option == 11)
                {
                    tls_io_instance->tls_version = VERSION_1_1;
                }
                else if (version_option == 12)
                {
                    tls_io_instance->tls_version = VERSION_1_2;
                }
                else
                {
                    LogInfo("Value of TLS version option %d is not found shall default to version 1.2", version_option);
                    tls_io_instance->tls_version = VERSION_1_2;
                }
                result = 0;
            }
        }
        else if (strcmp(optionName, OPTION_UNDERLYING_IO_OPTIONS) == 0)
        {
            if (OptionHandler_FeedOptions((OPTIONHANDLER_HANDLE)value, (void*)tls_io_instance->underlying_io) != OPTIONHANDLER_OK)
            {
                LogError("failed feeding options to underlying I/O instance");
                result = MU_FAILURE;
            }
            else
            {
                result = 0;
            }
        }
        else if (strcmp(optionName, OPTION_SET_TLS_RENEGOTIATION) == 0)
        {
            // No need to do anything for Openssl
            result = 0;
        }
        else if (strcmp("ignore_host_name_check", optionName) == 0)
        {
            bool* server_name_check = (bool*)value;
            tls_io_instance->ignore_host_name_check = *server_name_check;
            result = 0;
        }
        else
        {
            if (tls_io_instance->underlying_io == NULL)
            {
                result = MU_FAILURE;
            }
            else
            {
                result = xio_setoption(tls_io_instance->underlying_io, optionName, value);
            }
        }
    }

    return result;
}

const IO_INTERFACE_DESCRIPTION* tlsio_openssl_get_interface_description(void)
{
    return &tlsio_openssl_interface_description;
}
