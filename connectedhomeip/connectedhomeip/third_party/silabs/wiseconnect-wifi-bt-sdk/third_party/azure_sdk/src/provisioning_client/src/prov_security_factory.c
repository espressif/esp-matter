// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>
#include "azure_c_shared_utility/gballoc.h"
#include "azure_c_shared_utility/xlogging.h"
#include "azure_c_shared_utility/crt_abstractions.h"
#include "azure_prov_client/prov_security_factory.h"
#include "azure_prov_client/iothub_security_factory.h"

#include "hsm_client_data.h"

static SECURE_DEVICE_TYPE g_device_hsm_type = SECURE_DEVICE_TYPE_UNKNOWN;
static char* g_symm_key = NULL;
static char* g_symm_key_reg_name = NULL;

static IOTHUB_SECURITY_TYPE get_iothub_security_type(SECURE_DEVICE_TYPE sec_type)
{
    IOTHUB_SECURITY_TYPE ret;

    switch (sec_type)
    {
#if defined(HSM_TYPE_SAS_TOKEN)  || defined(HSM_AUTH_TYPE_CUSTOM)
        case SECURE_DEVICE_TYPE_TPM:
            ret = IOTHUB_SECURITY_TYPE_SAS;
            break;
#endif

#if defined(HSM_TYPE_X509) || defined(HSM_AUTH_TYPE_CUSTOM)
        case SECURE_DEVICE_TYPE_X509:
            ret = IOTHUB_SECURITY_TYPE_X509;
            break;
#endif

#if defined(HSM_TYPE_SYMM_KEY) || defined(HSM_AUTH_TYPE_CUSTOM)
        case SECURE_DEVICE_TYPE_SYMMETRIC_KEY:
            ret = IOTHUB_SECURITY_TYPE_SYMMETRIC_KEY;
            break;
#endif

#ifdef HSM_TYPE_HTTP_EDGE
        case SECURE_DEVICE_TYPE_HTTP_EDGE:
            ret = IOTHUB_SECURITY_TYPE_HTTP_EDGE;
            break;
#endif

        default:
            ret = IOTHUB_SECURITY_TYPE_UNKNOWN;
            break;
    }

    return ret;
}

int prov_dev_security_init(SECURE_DEVICE_TYPE hsm_type)
{
    int result;

    IOTHUB_SECURITY_TYPE security_type_from_caller = get_iothub_security_type(hsm_type);

    if (security_type_from_caller == IOTHUB_SECURITY_TYPE_UNKNOWN)
    {
        LogError("HSM type %d is not supported on this SDK build", hsm_type);
        result = MU_FAILURE;
    }
    else
    {
        g_device_hsm_type = hsm_type;
        IOTHUB_SECURITY_TYPE security_type_from_iot = iothub_security_type();
        if (security_type_from_iot == IOTHUB_SECURITY_TYPE_UNKNOWN)
        {
            // Initialize iothub_security layer if not currently
            result = iothub_security_init(security_type_from_caller);
        }
        else if (security_type_from_iot != security_type_from_caller)
        {
            LogError("Security HSM from caller %d (which maps to security type %d) does not match already specified security type %d", hsm_type, security_type_from_caller, security_type_from_iot);
            result = MU_FAILURE;
        }
        else
        {
            result = 0;
        }

        if (result == 0)
        {
            result = initialize_hsm_system();
        }
    }
    return result;
}

void prov_dev_security_deinit(void)
{
    if (g_symm_key != NULL)
    {
        free(g_symm_key);
        g_symm_key = NULL;
    }
    if (g_symm_key_reg_name != NULL)
    {
        free(g_symm_key_reg_name);
        g_symm_key_reg_name = NULL;
    }
    deinitialize_hsm_system();
    if (iothub_security_get_symmetric_key() != NULL || iothub_security_get_symm_registration_name() != NULL)
    {
        // Clear out iothub info
        iothub_security_deinit();
    }
}

SECURE_DEVICE_TYPE prov_dev_security_get_type(void)
{
    return g_device_hsm_type;
}

int prov_dev_set_symmetric_key_info(const char* registration_name, const char* symmetric_key)
{
    int result;
    if (registration_name == NULL || symmetric_key == NULL)
    {
        LogError("Invalid parameter specified reg_name: %p, symm_key: %p", registration_name, symmetric_key);
        result = MU_FAILURE;
    }
    else
    {
        char* temp_key;
        char* temp_name;
        if (mallocAndStrcpy_s(&temp_name, registration_name) != 0)
        {
            LogError("Failure allocating registration name");
            result = MU_FAILURE;
        }
        else if (mallocAndStrcpy_s(&temp_key, symmetric_key) != 0)
        {
            LogError("Failure allocating symmetric key");
            free(temp_name);
            result = MU_FAILURE;
        }
        else
        {
            if (g_symm_key != NULL)
            {
                free(g_symm_key);
            }
            if (g_symm_key_reg_name != NULL)
            {
                free(g_symm_key_reg_name);
            }
            g_symm_key_reg_name = temp_name;
            g_symm_key = temp_key;

            // Sync dps with iothub only if it is NULL
            if (iothub_security_get_symmetric_key() == NULL || iothub_security_get_symm_registration_name() == NULL)
            {
                if (iothub_security_set_symmetric_key_info(g_symm_key_reg_name, g_symm_key) != 0)
                {
                    LogError("Failure syncing dps & IoThub key information");
                    result = MU_FAILURE;
                }
                else
                {
                    result = 0;
                }
            }
            else
            {
                result = 0;
            }
        }
    }
    return result;
}

const char* prov_dev_get_symmetric_key(void)
{
    return g_symm_key;
}

const char* prov_dev_get_symm_registration_name(void)
{
    return g_symm_key_reg_name;
}
