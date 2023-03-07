// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "azure_c_shared_utility/gballoc.h"

#include "schemalib.h"
#include "codefirst.h"
#include "schema.h"
#include "datamarshaller.h"
#include "datapublisher.h"
#include <stddef.h>
#include "azure_c_shared_utility/xlogging.h"
#include "iotdevice.h"

#define DEFAULT_CONTAINER_NAME  "Container"

MU_DEFINE_ENUM_STRINGS_WITHOUT_INVALID(SERIALIZER_RESULT, SERIALIZER_RESULT_VALUES);

typedef enum AGENT_STATE_TAG
{
    AGENT_NOT_INITIALIZED,
    AGENT_INITIALIZED
} AGENT_STATE;

static AGENT_STATE g_AgentState = AGENT_NOT_INITIALIZED;

SERIALIZER_RESULT serializer_init(const char* overrideSchemaNamespace)
{
    SERIALIZER_RESULT result;

    /* Codes_SRS_SCHEMALIB_99_074:[serializer_init when already initialized shall return SERIALIZER_ALREADY_INIT.] */
    if (g_AgentState != AGENT_NOT_INITIALIZED)
    {
        result = SERIALIZER_ALREADY_INIT;
        LogError("(result = %s)", MU_ENUM_TO_STRING(SERIALIZER_RESULT, result));
    }
    else
    {
        /* Codes_SRS_SCHEMALIB_99_006:[ Initialize CodeFirst by a call to CodeFirst_Init.] */
        /* Codes_SRS_SCHEMALIB_99_076:[serializer_init shall pass the value of overrideSchemaNamespace argument to CodeFirst_Init.] */
        if (CodeFirst_Init(overrideSchemaNamespace) != CODEFIRST_OK)
        {
            /* Codes_SRS_SCHEMALIB_99_007:[ On error SERIALIZER_CODEFIRST_INIT_FAILED shall be returned.] */
            result = SERIALIZER_CODEFIRST_INIT_FAILED;
            LogError("(result = %s)", MU_ENUM_TO_STRING(SERIALIZER_RESULT, result));
        }
        else
        {
            /* Codes_SRS_SCHEMALIB_99_075:[When an serializer_init call fails for any reason the previous initialization state shall be preserved. The initialized state shall only be changed on a succesfull Init.] */
            g_AgentState = AGENT_INITIALIZED;

            /* Codes_SRS_SCHEMALIB_99_073:[On success serializer_init shall return SERIALIZER_OK.] */
            result = SERIALIZER_OK;
        }
    }

    return result;
}

void serializer_deinit(void)
{
    /* Codes_SRS_SCHEMALIB_99_025:[ serializer_deinit shall de-initialize all modules initialized by serializer_init.] */
    if (g_AgentState == AGENT_INITIALIZED)
    {
        CodeFirst_Deinit();
    }

    /* Codes_SRS_SCHEMALIB_99_026:[ A subsequent call to serializer_start shall succeed.] */
    g_AgentState = AGENT_NOT_INITIALIZED;
}

SERIALIZER_RESULT serializer_setconfig(SERIALIZER_CONFIG which, void* value)
{
    SERIALIZER_RESULT result;

    /* Codes_SRS_SCHEMALIB_99_137:[ If the value argument is NULL, serializer_setconfig shall return SERIALIZER_INVALID_ARG.] */
    if (value == NULL)
    {
        result = SERIALIZER_INVALID_ARG;
    }
    /* Codes_SRS_SCHEMALIB_99_142:[ When the which argument is SerializeDelayedBufferMaxSize, serializer_setconfig shall invoke DataPublisher_SetMaxBufferSize with the dereferenced value argument, and shall return SERIALIZER_OK.] */
    else if (which == SerializeDelayedBufferMaxSize)
    {
        DataPublisher_SetMaxBufferSize(*(size_t*)value);
        result = SERIALIZER_OK;
    }
    /* Codes_SRS_SCHEMALIB_99_138:[ If the which argument is not one of the declared members of the SERIALIZER_CONFIG enum, serializer_setconfig shall return SERIALIZER_INVALID_ARG.] */
    else
    {
        result = SERIALIZER_INVALID_ARG;
    }

    return result;
}
