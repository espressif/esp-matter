// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>
#include "azure_c_shared_utility/gballoc.h"

#include <stddef.h>
#include "schemaserializer.h"
#include "azure_c_shared_utility/xlogging.h"
#include "azure_macro_utils/macro_utils.h"

MU_DEFINE_ENUM_STRINGS_WITHOUT_INVALID(SCHEMA_SERIALIZER_RESULT, SCHEMA_SERIALIZER_RESULT_VALUES);

#define LOG_SCHEMA_SERIALIZER_ERROR(result) LogError("(result = %s)", MU_ENUM_TO_STRING(SCHEMA_SERIALIZER_RESULT, (result)))

static const char* ConvertType(const char* sourceType)
{
    /* Codes_SRS_SCHEMA_SERIALIZER_01_016: ["ascii_char_ptr" shall be translated to "string".] */
    if (strcmp(sourceType, "ascii_char_ptr") == 0)
    {
        return "string";
    }
    else
    {
        /* Codes_SRS_SCHEMA_SERIALIZER_01_017: [All other types shall be kept as they are.] */
        return sourceType;
    }
}

/* Codes_SRS_SCHEMA_SERIALIZER_01_001: [SchemaSerializer_SerializeCommandMetadata shall serialize a specific model to a string using JSON as format.] */
SCHEMA_SERIALIZER_RESULT SchemaSerializer_SerializeCommandMetadata(SCHEMA_MODEL_TYPE_HANDLE modelHandle, STRING_HANDLE schemaText)
{
    SCHEMA_SERIALIZER_RESULT result;

    /* Codes_SRS_SCHEMA_SERIALIZER_01_013: [If the modelHandle argument is NULL, SchemaSerializer_SerializeCommandMetadata shall return SCHEMA_SERIALIZER_INVALID_ARG.] */
    if ((modelHandle == NULL) ||
        /* Codes_SRS_SCHEMA_SERIALIZER_01_014: [If the schemaText argument is NULL, SchemaSerializer_SerializeCommandMetadata shall return SCHEMA_SERIALIZER_INVALID_ARG.] */
        (schemaText == NULL))
    {
        result = SCHEMA_SERIALIZER_INVALID_ARG;
        LogError("(result = %s), modelHandle = %p, schemaText = %p", MU_ENUM_TO_STRING(SCHEMA_SERIALIZER_RESULT, result), modelHandle, schemaText);
    }
    else
    {
        size_t commandCount;

        /* Codes_SRS_SCHEMA_SERIALIZER_01_002: [Only commands shall be serialized, the properties of a model shall be ignored.] */

        /* Codes_SRS_SCHEMA_SERIALIZER_01_003: [The output JSON shall have an array, where each array element shall represent a command.] */
        /* Codes_SRS_SCHEMA_SERIALIZER_01_011: [The JSON text shall be built into the string indicated by the schemaText argument by using String APIs.] */
        if ((STRING_concat(schemaText, "[") != 0) ||
            /* Codes_SRS_SCHEMA_SERIALIZER_01_006: [The object for a command shall have a member named Name, whose value shall be the command name as obtained by using Schema APIs.] */
            (Schema_GetModelActionCount(modelHandle, &commandCount) != SCHEMA_OK))
        {
            /* Codes_SRS_SCHEMA_SERIALIZER_01_015: [If any of the Schema or String APIs fail then SchemaSerializer_SerializeCommandMetadata shall return SCHEMA_SERIALIZER_ERROR.] */
            result = SCHEMA_SERIALIZER_ERROR;
            LOG_SCHEMA_SERIALIZER_ERROR(result);
        }
        else
        {
            size_t i;

            for (i = 0; i < commandCount; i++)
            {
                SCHEMA_ACTION_HANDLE actionHandle = Schema_GetModelActionByIndex(modelHandle, i);
                const char* commandName;
                size_t argCount;
                size_t j;

                /* Codes_SRS_SCHEMA_SERIALIZER_01_005: [Each array element shall be a JSON object.] */
                if ((actionHandle == NULL) ||
                    (STRING_concat(schemaText, "{ \"Name\":\"") != 0) ||
                    ((commandName = Schema_GetModelActionName(actionHandle)) == NULL) ||
                    (STRING_concat(schemaText, commandName) != 0) ||
                    /* Codes_SRS_SCHEMA_SERIALIZER_01_007: [The object for a command shall also have a "Parameters" member.] */
                    (STRING_concat(schemaText, "\", \"Parameters\":[") != 0) ||
                    (Schema_GetModelActionArgumentCount(actionHandle, &argCount) != SCHEMA_OK))
                {
                    /* Codes_SRS_SCHEMA_SERIALIZER_01_015: [If any of the Schema or String APIs fail then SchemaSerializer_SerializeCommandMetadata shall return SCHEMA_SERIALIZER_ERROR.] */
                    LogError("Failed encoding action.");
                    break;
                }
                else
                {
                    for (j = 0; j < argCount; j++)
                    {
                        /* Codes_SRS_SCHEMA_SERIALIZER_01_008: [The parameters member shall be an array, where each entry is a command parameter.] */
                        SCHEMA_ACTION_ARGUMENT_HANDLE argHandle = Schema_GetModelActionArgumentByIndex(actionHandle, j);
                        const char* argName;
                        const char* argType;

                        /* Codes_SRS_SCHEMA_SERIALIZER_01_009: [Each command parameter shall have a member named "Name", that should have as value the command argument name as obtained by using Schema APIs.] */
                        if ((argHandle == NULL) ||
                            (STRING_concat(schemaText, "{\"Name\":\"") != 0) ||
                            ((argName = Schema_GetActionArgumentName(argHandle)) == NULL) ||
                            (STRING_concat(schemaText, argName) != 0) ||
                            /* Codes_SRS_SCHEMA_SERIALIZER_01_010: [Each command parameter shall have a member named "Type", that should have as value the command argument type as obtained by using Schema APIs.] */
                            (STRING_concat(schemaText, "\",\"Type\":\"") != 0) ||
                            ((argType = Schema_GetActionArgumentType(argHandle)) == NULL) ||
                            (STRING_concat(schemaText, ConvertType(argType)) != 0))
                        {
                            /* Codes_SRS_SCHEMA_SERIALIZER_01_015: [If any of the Schema or String APIs fail then SchemaSerializer_SerializeCommandMetadata shall return SCHEMA_SERIALIZER_ERROR.] */
                            LogError("Failed encoding argument.");
                            break;
                        }
                        else
                        {
                            if (j + 1 < argCount)
                            {
                                if (STRING_concat(schemaText, "\"},") != 0)
                                {
                                    /* Codes_SRS_SCHEMA_SERIALIZER_01_015: [If any of the Schema or String APIs fail then SchemaSerializer_SerializeCommandMetadata shall return SCHEMA_SERIALIZER_ERROR.] */
                                    LogError("Failed to concatenate arg end.");
                                    break;
                                }
                            }
                            else
                            {
                                if (STRING_concat(schemaText, "\"}") != 0)
                                {
                                    /* Codes_SRS_SCHEMA_SERIALIZER_01_015: [If any of the Schema or String APIs fail then SchemaSerializer_SerializeCommandMetadata shall return SCHEMA_SERIALIZER_ERROR.] */
                                    LogError("Failed to concatenate arg end.");
                                    break;
                                }
                            }
                        }
                    }

                    if (j < argCount)
                    {
                        break;
                    }

                    if (i + 1 < commandCount)
                    {
                        if (STRING_concat(schemaText, "]},") != 0)
                        {
                            /* Codes_SRS_SCHEMA_SERIALIZER_01_015: [If any of the Schema or String APIs fail then SchemaSerializer_SerializeCommandMetadata shall return SCHEMA_SERIALIZER_ERROR.] */
                            LogError("Failed to concatenate.");
                            break;
                        }
                    }
                    else
                    {
                        if (STRING_concat(schemaText, "]}") != 0)
                        {
                            /* Codes_SRS_SCHEMA_SERIALIZER_01_015: [If any of the Schema or String APIs fail then SchemaSerializer_SerializeCommandMetadata shall return SCHEMA_SERIALIZER_ERROR.] */
                            LogError("Failed to concatenate.");
                            break;
                        }
                    }
                }
            }

            if (i < commandCount)
            {
                result = SCHEMA_SERIALIZER_ERROR;
            }
            else if (STRING_concat(schemaText, "]") != 0)
            {
                /* Codes_SRS_SCHEMA_SERIALIZER_01_015: [If any of the Schema or String APIs fail then SchemaSerializer_SerializeCommandMetadata shall return SCHEMA_SERIALIZER_ERROR.] */
                LogError("Failed to concatenate commands object end.");
                result = SCHEMA_SERIALIZER_ERROR;
            }
            else
            {
                /* Codes_SRS_SCHEMA_SERIALIZER_01_012: [On success SchemaSerializer_SerializeCommandMetadata shall return SCHEMA_SERIALIZER_OK.] */
                result = SCHEMA_SERIALIZER_OK;
            }
        }
    }

    return result;
}
