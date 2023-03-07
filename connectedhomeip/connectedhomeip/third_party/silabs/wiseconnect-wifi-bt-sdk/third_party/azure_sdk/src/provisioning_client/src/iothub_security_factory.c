// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>
#include "azure_prov_client/iothub_security_factory.h"
#include "azure_prov_client/prov_security_factory.h"
#include "azure_c_shared_utility/xlogging.h"
#include "azure_c_shared_utility/crt_abstractions.h"

#include "hsm_client_data.h"

static IOTHUB_SECURITY_TYPE g_security_type = IOTHUB_SECURITY_TYPE_UNKNOWN;
static char* g_symm_key = NULL;
static char* g_symm_key_reg_name = NULL;

static SECURE_DEVICE_TYPE get_secure_device_type(IOTHUB_SECURITY_TYPE sec_type)
{
    SECURE_DEVICE_TYPE ret;

    switch (sec_type)
    {
#if defined(HSM_TYPE_SAS_TOKEN)  || defined(HSM_AUTH_TYPE_CUSTOM)
        case IOTHUB_SECURITY_TYPE_SAS:
            ret = SECURE_DEVICE_TYPE_TPM;
            break;
#endif

#if defined(HSM_TYPE_X509) || defined(HSM_AUTH_TYPE_CUSTOM)
        case IOTHUB_SECURITY_TYPE_X509:
            ret = SECURE_DEVICE_TYPE_X509;
            break;
#endif

#if defined(HSM_TYPE_SYMM_KEY) || defined(HSM_AUTH_TYPE_CUSTOM)
        case IOTHUB_SECURITY_TYPE_SYMMETRIC_KEY:
            ret = SECURE_DEVICE_TYPE_SYMMETRIC_KEY;
            break;
#endif

#ifdef HSM_TYPE_HTTP_EDGE
        case IOTHUB_SECURITY_TYPE_HTTP_EDGE:
            ret = SECURE_DEVICE_TYPE_HTTP_EDGE;
            break;
#endif

        default:
            ret = SECURE_DEVICE_TYPE_UNKNOWN;
            break;
    }

    return ret;
}

int iothub_security_init(IOTHUB_SECURITY_TYPE sec_type)
{
    int result;

    SECURE_DEVICE_TYPE secure_device_type_from_caller = get_secure_device_type(sec_type);

    if (secure_device_type_from_caller == SECURE_DEVICE_TYPE_UNKNOWN)
    {
        LogError("Security type %d is not supported in this SDK build", sec_type);
        result = MU_FAILURE;
    }
    else
    {
        g_security_type = sec_type;
        SECURE_DEVICE_TYPE security_device_type_from_prov = prov_dev_security_get_type();
        if (security_device_type_from_prov == SECURE_DEVICE_TYPE_UNKNOWN)
        {
            result = prov_dev_security_init(secure_device_type_from_caller);
        }
        else if (secure_device_type_from_caller != security_device_type_from_prov)
        {
            LogError("Security type from caller %d (which maps to security device type %d) does not match already specified security device type %d", sec_type, secure_device_type_from_caller, security_device_type_from_prov);
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

void iothub_security_deinit()
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
    if (prov_dev_get_symmetric_key() != NULL || prov_dev_get_symm_registration_name() != NULL)
    {
        prov_dev_security_deinit();
    }
}

IOTHUB_SECURITY_TYPE iothub_security_type()
{
    return g_security_type;
}

int iothub_security_set_symmetric_key_info(const char* registration_name, const char* symmetric_key)
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

            // Sync iothub with dps
            if (prov_dev_get_symmetric_key() == NULL || prov_dev_get_symm_registration_name() == NULL)
            {
                if (prov_dev_set_symmetric_key_info(g_symm_key_reg_name, g_symm_key) != 0)
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

const char* iothub_security_get_symmetric_key()
{
    return g_symm_key;
}

const char* iothub_security_get_symm_registration_name()
{
    return g_symm_key_reg_name;
}
