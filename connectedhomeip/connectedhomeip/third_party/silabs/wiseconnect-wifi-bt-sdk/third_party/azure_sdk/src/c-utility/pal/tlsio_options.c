// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>

#include "azure_c_shared_utility/gballoc.h"
#include "azure_c_shared_utility/tlsio_options.h"
#include "azure_c_shared_utility/shared_util_options.h"
#include "azure_c_shared_utility/xlogging.h"
#include "azure_c_shared_utility/crt_abstractions.h"


// Initialize the TLSIO_OPTIONS struct
void tlsio_options_initialize(TLSIO_OPTIONS* options, int supported_options)
{
    // Using static function rules, so 'options' is not checked for NULL
    //
    // The supported_options value does not need validation because undefined bits are
    // ignored, while any valid missing bits result in an "option not supported" error
    // that will show up in unit testing.
    options->supported_options = supported_options;
    options->trusted_certs = NULL;
    options->x509_type = TLSIO_OPTIONS_x509_TYPE_UNSPECIFIED;
    options->x509_cert = NULL;
    options->x509_key = NULL;
}

static int set_and_validate_x509_type(TLSIO_OPTIONS* options, TLSIO_OPTIONS_x509_TYPE x509_type)
{
    int result;
    if ((options->supported_options & x509_type) == 0)
    {
        // This case also rejects the nonsensical TLSIO_OPTIONS_x509_TYPE_UNSPECIFIED
        LogError("Unsupported x509 type: %d", x509_type);
        result = MU_FAILURE;
    }
    else if (options->x509_type == TLSIO_OPTIONS_x509_TYPE_UNSPECIFIED)
    {
        // Initial type setting
        options->x509_type = x509_type;
        result = 0;
    }
    else if (options->x509_type != x509_type)
    {
        LogError("Supplied x509 type conflicts with previously set x509");
        result = MU_FAILURE;
    }
    else
    {
        // The types match okay
        result = 0;
    }

    return result;
}

void tlsio_options_release_resources(TLSIO_OPTIONS* options)
{
    if (options != NULL)
    {
        free((void*)options->trusted_certs);
        free((void*)options->x509_cert);
        free((void*)options->x509_key);
    }
    else
    {
        LogError("NULL options");
    }
}

static bool is_supported_string_option(const char* name)
{
    return
        (strcmp(name, OPTION_TRUSTED_CERT) == 0) ||
        (strcmp(name, OPTION_OPENSSL_CIPHER_SUITE) == 0) ||
        (strcmp(name, OPTION_OPENSSL_ENGINE) == 0) ||
        (strcmp(name, OPTION_OPENSSL_PRIVATE_KEY_TYPE) == 0) ||
        (strcmp(name, SU_OPTION_X509_CERT) == 0) ||
        (strcmp(name, SU_OPTION_X509_PRIVATE_KEY) == 0) ||
        (strcmp(name, OPTION_X509_ECC_CERT) == 0) ||
        (strcmp(name, OPTION_X509_ECC_KEY) == 0);
}

TLSIO_OPTIONS_RESULT tlsio_options_destroy_option(const char* name, const void* value)
{
    TLSIO_OPTIONS_RESULT result;
    if (name == NULL || value == NULL)
    {
        LogError("NULL parameter: name: %p, value: %p", name, value);
        result = TLSIO_OPTIONS_RESULT_ERROR;
    }
    else if (is_supported_string_option(name))
    {
        free((void*)value);
        result = TLSIO_OPTIONS_RESULT_SUCCESS;
    }
    else
    {
        result = TLSIO_OPTIONS_RESULT_NOT_HANDLED;
    }
    return result;
}

TLSIO_OPTIONS_RESULT tlsio_options_clone_option(const char* name, const void* value, void** out_value)
{
    TLSIO_OPTIONS_RESULT result;

    if (name == NULL || value == NULL || out_value == NULL)
    {
        LogError("NULL parameter: name: %p, value: %p, out_value: %p",
            name, value, out_value);
        result = TLSIO_OPTIONS_RESULT_ERROR;
    }
    else if (is_supported_string_option(name))
    {
        *out_value = NULL;
        if (mallocAndStrcpy_s((char**)out_value, value) != 0)
        {
            LogError("unable to mallocAndStrcpy_s option value");
            result = TLSIO_OPTIONS_RESULT_ERROR;
        }
        else
        {
            result = TLSIO_OPTIONS_RESULT_SUCCESS;
        }
    }
    else
    {
        result = TLSIO_OPTIONS_RESULT_NOT_HANDLED;
    }
    return result;
}

TLSIO_OPTIONS_RESULT tlsio_options_set(TLSIO_OPTIONS* options,
    const char* optionName, const void* value)
{
    TLSIO_OPTIONS_RESULT result;
    char* copied_value = NULL;

    if (options == NULL || optionName == NULL || value == NULL)
    {
        LogError("NULL parameter: options: %p, optionName: %p, value: %p",
            options, optionName, value);
        result = TLSIO_OPTIONS_RESULT_ERROR;
    }
    else if (!is_supported_string_option(optionName))
    {
        result = TLSIO_OPTIONS_RESULT_NOT_HANDLED;
    }
    else if(mallocAndStrcpy_s(&copied_value, value) != 0)
    {
        LogError("unable to mallocAndStrcpy_s option value");
        result = TLSIO_OPTIONS_RESULT_ERROR;
    }
    else if (strcmp(OPTION_TRUSTED_CERT, optionName) == 0)
    {
        if ((options->supported_options & TLSIO_OPTION_BIT_TRUSTED_CERTS) == 0)
        {
            LogError("Trusted certs option not supported");
            result = TLSIO_OPTIONS_RESULT_ERROR;
        }
        else if (options->trusted_certs != NULL)
        {
            LogError("unable to set trusted cert option more than once");
            result = TLSIO_OPTIONS_RESULT_ERROR;
        }
        else
        {
            options->trusted_certs = copied_value;
            result = TLSIO_OPTIONS_RESULT_SUCCESS;
        }
    }
    else if (strcmp(SU_OPTION_X509_CERT, optionName) == 0 || strcmp(OPTION_X509_ECC_CERT, optionName) == 0)
    {
        TLSIO_OPTIONS_x509_TYPE this_type = (strcmp(SU_OPTION_X509_CERT, optionName) == 0) ? TLSIO_OPTIONS_x509_TYPE_RSA : TLSIO_OPTIONS_x509_TYPE_ECC;
        if (options->x509_cert != NULL)
        {
            LogError("unable to set x509 cert more than once");
            result = TLSIO_OPTIONS_RESULT_ERROR;
        }
        else if (set_and_validate_x509_type(options, this_type) != 0)
        {
            // Error logged by helper
            result = TLSIO_OPTIONS_RESULT_ERROR;
        }
        else
        {
            options->x509_cert = copied_value;
            result = TLSIO_OPTIONS_RESULT_SUCCESS;
        }
    }
    else if (strcmp(SU_OPTION_X509_PRIVATE_KEY, optionName) == 0 || strcmp(OPTION_X509_ECC_KEY, optionName) == 0)
    {
        TLSIO_OPTIONS_x509_TYPE this_type = (strcmp(SU_OPTION_X509_PRIVATE_KEY, optionName) == 0) ? TLSIO_OPTIONS_x509_TYPE_RSA : TLSIO_OPTIONS_x509_TYPE_ECC;
        if (options->x509_key != NULL)
        {
            LogError("unable to set x509 key more than once");
            result = TLSIO_OPTIONS_RESULT_ERROR;
        }
        else if (set_and_validate_x509_type(options, this_type) != 0)
        {
            // Error logged by helper
            result = TLSIO_OPTIONS_RESULT_ERROR;
        }
        else
        {
            options->x509_key = copied_value;
            result = TLSIO_OPTIONS_RESULT_SUCCESS;
        }
    }
    else
    {
        // This is logically impossible due to earlier tests, so just quiet the compiler
        result = TLSIO_OPTIONS_RESULT_ERROR;
    }

    if (result != TLSIO_OPTIONS_RESULT_SUCCESS)
    {
        free(copied_value);
    }

    return result;
}

// A helper that works if the tlsio does not use any extra options
static void* local_clone_option(const char* name, const void* value)
{
    void* result = NULL;
    if (tlsio_options_clone_option(name, value, &result) != TLSIO_OPTIONS_RESULT_SUCCESS)
    {
        LogError("Unexpected local_clone_option failure");
    }
    return result;
}

// A helper that works if the tlsio does not use any extra options
void local_destroy_option(const char* name, const void* value)
{
    if (tlsio_options_destroy_option(name, value) != TLSIO_OPTIONS_RESULT_SUCCESS)
    {
        LogError("Unexpected local_destroy_option failure");
    }
}

OPTIONHANDLER_HANDLE tlsio_options_retrieve_options(TLSIO_OPTIONS* options, pfSetOption setOption)
{
    return tlsio_options_retrieve_options_ex(options, local_clone_option, local_destroy_option, setOption);
}


OPTIONHANDLER_HANDLE tlsio_options_retrieve_options_ex(TLSIO_OPTIONS* options,
    pfCloneOption cloneOption, pfDestroyOption destroyOption, pfSetOption setOption)
{
    OPTIONHANDLER_HANDLE result;
    if (options == NULL || cloneOption == NULL || destroyOption == NULL || setOption == NULL)
    {
        LogError("Null parameter in options: %p, cloneOption: %p, destroyOption: %p, setOption: %p",
            options, cloneOption, destroyOption, setOption);
        result = NULL;
    }
    else
    {
        result = OptionHandler_Create(cloneOption, destroyOption, setOption);
        if (result == NULL)
        {
            LogError("OptionHandler_Create failed");
            /*return as is*/
        }
        else if (
                (options->trusted_certs != NULL) &&
                (OptionHandler_AddOption(result, OPTION_TRUSTED_CERT, options->trusted_certs) != OPTIONHANDLER_OK)
                )
        {
            LogError("unable to save TrustedCerts option");
            OptionHandler_Destroy(result);
            result = NULL;
        }
        else if (options->x509_type != TLSIO_OPTIONS_x509_TYPE_UNSPECIFIED)
        {
            const char* x509_cert_option;
            const char* x509_key_option;
            if (options->x509_type == TLSIO_OPTIONS_x509_TYPE_ECC)
            {
                x509_cert_option = OPTION_X509_ECC_CERT;
                x509_key_option = OPTION_X509_ECC_KEY;
            }
            else
            {
                x509_cert_option = SU_OPTION_X509_CERT;
                x509_key_option = SU_OPTION_X509_PRIVATE_KEY;
            }
            if (
                (options->x509_cert != NULL) &&
                (OptionHandler_AddOption(result, x509_cert_option, options->x509_cert) != OPTIONHANDLER_OK)
                )
            {
                LogError("unable to save x509 cert option");
                OptionHandler_Destroy(result);
                result = NULL;
            }
            else if (
                (options->x509_key != NULL) &&
                (OptionHandler_AddOption(result, x509_key_option, options->x509_key) != OPTIONHANDLER_OK)
                )
            {
                LogError("unable to save x509 key option");
                OptionHandler_Destroy(result);
                result = NULL;
            }
        }
    }

    return result;
}

