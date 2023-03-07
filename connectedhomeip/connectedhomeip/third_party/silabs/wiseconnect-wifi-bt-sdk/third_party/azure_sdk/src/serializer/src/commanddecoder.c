// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>
#include "azure_c_shared_utility/optimize_size.h"
#include "azure_c_shared_utility/gballoc.h"

#include <stddef.h>

#include "commanddecoder.h"
#include "multitree.h"
#include "azure_c_shared_utility/crt_abstractions.h"
#include "azure_c_shared_utility/xlogging.h"
#include "schema.h"
#include "codefirst.h"
#include "jsondecoder.h"

MU_DEFINE_ENUM_STRINGS_WITHOUT_INVALID(COMMANDDECODER_RESULT, COMMANDDECODER_RESULT_VALUES);

typedef struct COMMAND_DECODER_HANDLE_DATA_TAG
{
    METHOD_CALLBACK_FUNC methodCallback;
    void* methodCallbackContext;
    SCHEMA_MODEL_TYPE_HANDLE ModelHandle;
    ACTION_CALLBACK_FUNC ActionCallback;
    void* ActionCallbackContext;
} COMMAND_DECODER_HANDLE_DATA;

static int DecodeValueFromNode(SCHEMA_HANDLE schemaHandle, AGENT_DATA_TYPE* agentDataType, MULTITREE_HANDLE node, const char* edmTypeName)
{
    /* because "pottentially uninitialized variable on MS compiler" */
    int result = 0;
    const char* argStringValue;
    AGENT_DATA_TYPE_TYPE primitiveType;

    /* Codes_SRS_COMMAND_DECODER_99_029:[ If the argument type is complex then a complex type value shall be built from the child nodes.] */
    if ((primitiveType = CodeFirst_GetPrimitiveType(edmTypeName)) == EDM_NO_TYPE)
    {
        SCHEMA_STRUCT_TYPE_HANDLE structTypeHandle;
        size_t propertyCount;

        /* Codes_SRS_COMMAND_DECODER_99_033:[ In order to determine which are the members of a complex types, Schema APIs for structure types shall be used.] */
        if (((structTypeHandle = Schema_GetStructTypeByName(schemaHandle, edmTypeName)) == NULL) ||
            (Schema_GetStructTypePropertyCount(structTypeHandle, &propertyCount) != SCHEMA_OK))
        {
            /* Codes_SRS_COMMAND_DECODER_99_010:[ If any Schema API fails then the command shall not be dispatched and it shall return EXECUTE_COMMAND_ERROR.]*/
            result = MU_FAILURE;
            LogError("Getting Struct information failed.");
        }
        else
        {
            if (propertyCount == 0)
            {
                /* Codes_SRS_COMMAND_DECODER_99_034:[ If Schema APIs indicate that a complex type has 0 members then the command shall not be dispatched and it shall return EXECUTE_COMMAND_ERROR.] */
                result = MU_FAILURE;
                LogError("Struct type with 0 members is not allowed");
            }
            else
            {
                AGENT_DATA_TYPE* memberValues = (AGENT_DATA_TYPE*)calloc(1, (sizeof(AGENT_DATA_TYPE)* propertyCount));
                if (memberValues == NULL)
                {
                    /* Codes_SRS_COMMAND_DECODER_99_021:[ If the parsing of the command fails for any other reason the command shall not be dispatched.] */
                    result = MU_FAILURE;
                    LogError("Failed allocating member values for command argument");
                }
                else
                {
                    const char** memberNames = (const char**)malloc(sizeof(const char*)* propertyCount);
                    if (memberNames == NULL)
                    {
                        /* Codes_SRS_COMMAND_DECODER_99_021:[ If the parsing of the command fails for any other reason the command shall not be dispatched.] */
                        result = MU_FAILURE;
                        LogError("Failed allocating member names for command argument.");
                    }
                    else
                    {
                        size_t j;
                        size_t k;

                        for (j = 0; j < propertyCount; j++)
                        {
                            SCHEMA_PROPERTY_HANDLE propertyHandle;
                            MULTITREE_HANDLE memberNode;
                            const char* propertyName;
                            const char* propertyType;

                            if ((propertyHandle = Schema_GetStructTypePropertyByIndex(structTypeHandle, j)) == NULL)
                            {
                                /* Codes_SRS_COMMAND_DECODER_99_010:[ If any Schema API fails then the command shall not be dispatched and it shall return EXECUTE_COMMAND_ERROR.]*/
                                result = MU_FAILURE;
                                LogError("Getting struct member failed.");
                                break;
                            }
                            else if (((propertyName = Schema_GetPropertyName(propertyHandle)) == NULL) ||
                                     ((propertyType = Schema_GetPropertyType(propertyHandle)) == NULL))
                            {
                                /* Codes_SRS_COMMAND_DECODER_99_010:[ If any Schema API fails then the command shall not be dispatched and it shall return EXECUTE_COMMAND_ERROR.]*/
                                result = MU_FAILURE;
                                LogError("Getting the struct member information failed.");
                                break;
                            }
                            else
                            {
                                memberNames[j] = propertyName;

                                /* Codes_SRS_COMMAND_DECODER_01_014: [CommandDecoder shall use the MultiTree APIs to extract a specific element from the command JSON.] */
                                if (MultiTree_GetChildByName(node, memberNames[j], &memberNode) != MULTITREE_OK)
                                {
                                    /* Codes_SRS_COMMAND_DECODER_99_028:[ If decoding the argument fails then the command shall not be dispatched and it shall return EXECUTE_COMMAND_ERROR.] */
                                    result = MU_FAILURE;
                                    LogError("Getting child %s failed", propertyName);
                                    break;
                                }
                                /* Codes_SRS_COMMAND_DECODER_99_032:[ Nesting shall be supported for complex type.] */
                                else if ((result = DecodeValueFromNode(schemaHandle, &memberValues[j], memberNode, propertyType)) != 0)
                                {
                                    break;
                                }
                            }
                        }

                        if (j == propertyCount)
                        {
                            /* Codes_SRS_COMMAND_DECODER_99_031:[ The complex type value that aggregates the children shall be built by using the Create_AGENT_DATA_TYPE_from_Members.] */
                            if (Create_AGENT_DATA_TYPE_from_Members(agentDataType, edmTypeName, propertyCount, (const char* const*)memberNames, memberValues) != AGENT_DATA_TYPES_OK)
                            {
                                /* Codes_SRS_COMMAND_DECODER_99_028:[ If decoding the argument fails then the command shall not be dispatched and it shall return EXECUTE_COMMAND_ERROR.] */
                                result = MU_FAILURE;
                                LogError("Creating the agent data type from members failed.");
                            }
                            else
                            {
                                result = 0;
                            }
                        }

                        for (k = 0; k < j; k++)
                        {
                            Destroy_AGENT_DATA_TYPE(&memberValues[k]);
                        }

                        free((void*)memberNames);
                    }

                    free(memberValues);
                }
            }
        }
    }
    else
    {
        /* Codes_SRS_COMMAND_DECODER_01_014: [CommandDecoder shall use the MultiTree APIs to extract a specific element from the command JSON.] */
        if (MultiTree_GetValue(node, (const void **)&argStringValue) != MULTITREE_OK)
        {
            /* Codes_SRS_COMMAND_DECODER_99_012:[ If any argument is missing in the command text then the command shall not be dispatched and it shall return EXECUTE_COMMAND_ERROR.] */
            result = MU_FAILURE;
            LogError("Getting the string from the multitree failed.");
        }
        /* Codes_SRS_COMMAND_DECODER_99_027:[ The value for an argument of primitive type shall be decoded by using the CreateAgentDataType_From_String API.] */
        else if (CreateAgentDataType_From_String(argStringValue, primitiveType, agentDataType) != AGENT_DATA_TYPES_OK)
        {
            /* Codes_SRS_COMMAND_DECODER_99_028:[ If decoding the argument fails then the command shall not be dispatched and it shall return EXECUTE_COMMAND_ERROR.] */
            result = MU_FAILURE;
            LogError("Failed parsing node %s.", argStringValue);
        }
    }

    return result;
}

static EXECUTE_COMMAND_RESULT DecodeAndExecuteModelAction(COMMAND_DECODER_HANDLE_DATA* commandDecoderInstance, SCHEMA_HANDLE schemaHandle, SCHEMA_MODEL_TYPE_HANDLE modelHandle, const char* relativeActionPath, const char* actionName, MULTITREE_HANDLE commandNode)
{
    EXECUTE_COMMAND_RESULT result;
    char tempStr[128];
    size_t strLength = strlen(actionName);

    if (strLength <= 1)
    {
        /* Codes_SRS_COMMAND_DECODER_99_021:[ If the parsing of the command fails for any other reason the command shall not be dispatched.] */
        LogError("Invalid action name");
        result = EXECUTE_COMMAND_ERROR;
    }
    else if (strLength >= 128)
    {
        /* Codes_SRS_COMMAND_DECODER_99_021:[ If the parsing of the command fails for any other reason the command shall not be dispatched.] */
        LogError("Invalid action name length");
        result = EXECUTE_COMMAND_ERROR;
    }
    else
    {
        /* Codes_SRS_COMMAND_DECODER_99_006:[ The action name shall be decoded from the element "Name" of the command JSON.] */
        SCHEMA_ACTION_HANDLE modelActionHandle;
        size_t argCount;
        MULTITREE_HANDLE parametersTreeNode;

        if (memcpy(tempStr, actionName, strLength-1) == NULL)
        {
            /* Codes_SRS_COMMAND_DECODER_99_021:[ If the parsing of the command fails for any other reason the command shall not be dispatched.] */
            LogError("Invalid action name.");
            result = EXECUTE_COMMAND_ERROR;
        }
        /* Codes_SRS_COMMAND_DECODER_01_014: [CommandDecoder shall use the MultiTree APIs to extract a specific element from the command JSON.] */
        else if (MultiTree_GetChildByName(commandNode, "Parameters", &parametersTreeNode) != MULTITREE_OK)
        {
            /* Codes_SRS_COMMAND_DECODER_01_015: [If any MultiTree API call fails then the processing shall stop and the command shall not be dispatched and it shall return EXECUTE_COMMAND_ERROR.] */
            LogError("Error getting Parameters node.");
            result = EXECUTE_COMMAND_ERROR;
        }
        else
        {
            tempStr[strLength-1] = 0;
            /* Codes_SRS_COMMAND_DECODER_99_009:[ CommandDecoder shall call Schema_GetModelActionByName to obtain the information about a specific action.] */
            if (((modelActionHandle = Schema_GetModelActionByName(modelHandle, tempStr)) == NULL) ||
                (Schema_GetModelActionArgumentCount(modelActionHandle, &argCount) != SCHEMA_OK))
            {
                /* Codes_SRS_COMMAND_DECODER_99_010:[ If any Schema API fails then the command shall not be dispatched and it shall return EXECUTE_COMMAND_ERROR.]*/
                LogError("Failed reading action %s from the schema", tempStr);
                result = EXECUTE_COMMAND_ERROR;
            }
            else
            {
                AGENT_DATA_TYPE* arguments = NULL;

                if (argCount > 0)
                {
                    arguments = (AGENT_DATA_TYPE*)calloc(1, (sizeof(AGENT_DATA_TYPE)* argCount));
                }

                if ((argCount > 0) &&
                    (arguments == NULL))
                {
                    /* Codes_SRS_COMMAND_DECODER_99_021:[ If the parsing of the command fails for any other reason the command shall not be dispatched.] */
                    LogError("Failed allocating arguments array");
                    result = EXECUTE_COMMAND_ERROR;
                }
                else
                {
                    size_t i;
                    size_t j;
                    result = EXECUTE_COMMAND_ERROR;

                    /* Codes_SRS_COMMAND_DECODER_99_011:[ CommandDecoder shall attempt to extract from the command text the value for each action argument.] */
                    for (i = 0; i < argCount; i++)
                    {
                        SCHEMA_ACTION_ARGUMENT_HANDLE actionArgumentHandle;
                        MULTITREE_HANDLE argumentNode;
                        const char* argName;
                        const char* argType;

                        if (((actionArgumentHandle = Schema_GetModelActionArgumentByIndex(modelActionHandle, i)) == NULL) ||
                            ((argName = Schema_GetActionArgumentName(actionArgumentHandle)) == NULL) ||
                            ((argType = Schema_GetActionArgumentType(actionArgumentHandle)) == NULL))
                        {
                            LogError("Failed getting the argument information from the schema");
                            result = EXECUTE_COMMAND_ERROR;
                            break;
                        }
                        /* Codes_SRS_COMMAND_DECODER_01_014: [CommandDecoder shall use the MultiTree APIs to extract a specific element from the command JSON.] */
                        /* Codes_SRS_COMMAND_DECODER_01_008: [Each argument shall be looked up as a field, member of the "Parameters" node.]  */
                        else if (MultiTree_GetChildByName(parametersTreeNode, argName, &argumentNode) != MULTITREE_OK)
                        {
                            /* Codes_SRS_COMMAND_DECODER_99_012:[ If any argument is missing in the command text then the command shall not be dispatched and it shall return EXECUTE_COMMAND_ERROR.] */
                            LogError("Missing argument %s", argName);
                            result = EXECUTE_COMMAND_ERROR;
                            break;
                        }
                        else if (DecodeValueFromNode(schemaHandle, &arguments[i], argumentNode, argType) != 0)
                        {
                            result = EXECUTE_COMMAND_ERROR;
                            break;
                        }
                    }

                    if (i == argCount)
                    {
                        /* Codes_SRS_COMMAND_DECODER_99_005:[ If an Invoke Action is decoded successfully then the callback actionCallback shall be called, passing to it the callback action context, decoded name and arguments.] */
                        result = commandDecoderInstance->ActionCallback(commandDecoderInstance->ActionCallbackContext, relativeActionPath, tempStr, argCount, arguments);
                    }

                    for (j = 0; j < i; j++)
                    {
                        Destroy_AGENT_DATA_TYPE(&arguments[j]);
                    }

                    if (arguments != NULL)
                    {
                        free(arguments);
                    }
                }
            }
        }
    }
    return result;
}

static METHODRETURN_HANDLE DecodeAndExecuteModelMethod(COMMAND_DECODER_HANDLE_DATA* commandDecoderInstance, SCHEMA_HANDLE schemaHandle, SCHEMA_MODEL_TYPE_HANDLE modelHandle, const char* relativeMethodPath, const char* methodName, MULTITREE_HANDLE methodTree)
{
    METHODRETURN_HANDLE result;
    size_t strLength = strlen(methodName);

    if (strLength == 0)
    {
        /*Codes_SRS_COMMAND_DECODER_02_023: [ If any of the previous operations fail, then CommandDecoder_ExecuteMethod shall return NULL. ]*/
        LogError("Invalid method name");
        result = NULL;
    }
    else
    {
        SCHEMA_METHOD_HANDLE modelMethodHandle;
        size_t argCount;

#ifdef _MSC_VER
#pragma warning(suppress: 6324) /* We intentionally use here strncpy */
#endif

        /*Codes_SRS_COMMAND_DECODER_02_020: [ CommandDecoder_ExecuteMethod shall verify that the model has a method called methodName. ]*/
        if (((modelMethodHandle = Schema_GetModelMethodByName(modelHandle, methodName)) == NULL) ||
            (Schema_GetModelMethodArgumentCount(modelMethodHandle, &argCount) != SCHEMA_OK))
        {
            /*Codes_SRS_COMMAND_DECODER_02_023: [ If any of the previous operations fail, then CommandDecoder_ExecuteMethod shall return NULL. ]*/
            LogError("Failed reading method %s from the schema", methodName);
            result = NULL;
        }
        else
        {
            /*Codes_SRS_COMMAND_DECODER_02_021: [ For every argument of methodName, CommandDecoder_ExecuteMethod shall build an AGENT_DATA_TYPE from the node with the same name from the MULTITREE_HANDLE. ]*/

            if (argCount == 0)
            {
                /*no need for any parameters*/
                result = commandDecoderInstance->methodCallback(commandDecoderInstance->methodCallbackContext, relativeMethodPath, methodName, 0, NULL);
            }
            else
            {
                AGENT_DATA_TYPE* arguments;
                arguments = (AGENT_DATA_TYPE*)calloc(1, (sizeof(AGENT_DATA_TYPE)* argCount));
                if (arguments == NULL)
                {
                    LogError("Failed allocating arguments array");
                    result = NULL;
                }
                else
                {
                    size_t i;
                    size_t j;
                    result = NULL;

                    for (i = 0; i < argCount; i++)
                    {
                        SCHEMA_METHOD_ARGUMENT_HANDLE methodArgumentHandle;
                        MULTITREE_HANDLE argumentNode;
                        const char* argName;
                        const char* argType;

                        if (((methodArgumentHandle = Schema_GetModelMethodArgumentByIndex(modelMethodHandle, i)) == NULL) ||
                            ((argName = Schema_GetMethodArgumentName(methodArgumentHandle)) == NULL) ||
                            ((argType = Schema_GetMethodArgumentType(methodArgumentHandle)) == NULL))
                        {
                            /*Codes_SRS_COMMAND_DECODER_02_023: [ If any of the previous operations fail, then CommandDecoder_ExecuteMethod shall return NULL. ]*/
                            LogError("Failed getting the argument information from the schema");
                            result = NULL;
                            break;
                        }
                        else if (MultiTree_GetChildByName(methodTree, argName, &argumentNode) != MULTITREE_OK)
                        {
                            /*Codes_SRS_COMMAND_DECODER_02_023: [ If any of the previous operations fail, then CommandDecoder_ExecuteMethod shall return NULL. ]*/
                            LogError("Missing argument %s", argName);
                            result = NULL;
                            break;
                        }
                        else if (DecodeValueFromNode(schemaHandle, &arguments[i], argumentNode, argType) != 0)
                        {
                            /*Codes_SRS_COMMAND_DECODER_02_023: [ If any of the previous operations fail, then CommandDecoder_ExecuteMethod shall return NULL. ]*/
                            LogError("failure in DecodeValueFromNode");
                            result = NULL;
                            break;
                        }
                    }

                    if (i == argCount)
                    {
                        /*Codes_SRS_COMMAND_DECODER_02_022: [ CommandDecoder_ExecuteMethod shall call methodCallback passing the context, the methodName, number of arguments and the AGENT_DATA_TYPE. ]*/
                        /*Codes_SRS_COMMAND_DECODER_02_024: [ Otherwise, CommandDecoder_ExecuteMethod shall return what methodCallback returns. ]*/
                        result = commandDecoderInstance->methodCallback(commandDecoderInstance->methodCallbackContext, relativeMethodPath, methodName, argCount, arguments);
                    }

                    for (j = 0; j < i; j++)
                    {
                        Destroy_AGENT_DATA_TYPE(&arguments[j]);
                    }

                    free(arguments);
                }

            }
        }

    }
    return result;
}


static EXECUTE_COMMAND_RESULT ScanActionPathAndExecuteAction(COMMAND_DECODER_HANDLE_DATA* commandDecoderInstance, SCHEMA_HANDLE schemaHandle, const char* actionPath, MULTITREE_HANDLE commandNode)
{
    EXECUTE_COMMAND_RESULT result;
    char* relativeActionPath;
    const char* actionName = actionPath;
    SCHEMA_MODEL_TYPE_HANDLE modelHandle = commandDecoderInstance->ModelHandle;

    /* Codes_SRS_COMMAND_DECODER_99_035:[ CommandDecoder_ExecuteCommand shall support paths to actions that are in child models (i.e. ChildModel/SomeAction.] */
    do
    {
        /* find the slash */
        const char* slashPos = strchr(actionName, '/');
        if (slashPos == NULL)
        {
            size_t relativeActionPathLength;

            /* Codes_SRS_COMMAND_DECODER_99_037:[ The relative path passed to the actionCallback shall be in the format "childModel1/childModel2/.../childModelN".] */
            if (actionName == actionPath)
            {
                relativeActionPathLength = 0;
            }
            else
            {
                relativeActionPathLength = actionName - actionPath - 1;
            }

            relativeActionPath = (char*)malloc(relativeActionPathLength + 1);
            if (relativeActionPath == NULL)
            {
                /* Codes_SRS_COMMAND_DECODER_99_021:[ If the parsing of the command fails for any other reason the command shall not be dispatched.] */
                LogError("Failed allocating relative action path");
                result = EXECUTE_COMMAND_ERROR;
            }
            else
            {
                strncpy(relativeActionPath, actionPath, relativeActionPathLength);
                relativeActionPath[relativeActionPathLength] = 0;

                /* no slash found, this must be an action */
                result = DecodeAndExecuteModelAction(commandDecoderInstance, schemaHandle, modelHandle, relativeActionPath, actionName, commandNode);

                free(relativeActionPath);
                actionName = NULL;
            }
            break;
        }
        else
        {
            /* found a slash, get the child model name */
            size_t modelLength = slashPos - actionName;
            char* childModelName = (char*)malloc(modelLength + 1);
            if (childModelName == NULL)
            {
                /* Codes_SRS_COMMAND_DECODER_99_021:[ If the parsing of the command fails for any other reason the command shall not be dispatched.] */
                LogError("Failed allocating child model name");
                result = EXECUTE_COMMAND_ERROR;
                break;
            }
            else
            {
                strncpy(childModelName, actionName, modelLength);
                childModelName[modelLength] = 0;

                /* find the model */
                modelHandle = Schema_GetModelModelByName(modelHandle, childModelName);
                if (modelHandle == NULL)
                {
                    /* Codes_SRS_COMMAND_DECODER_99_036:[ If a child model cannot be found by using Schema APIs then the command shall not be dispatched and it shall return EXECUTE_COMMAND_ERROR.] */
                    LogError("Getting the model %s failed", childModelName);
                    free(childModelName);
                    result = EXECUTE_COMMAND_ERROR;
                    break;
                }
                else
                {
                    free(childModelName);
                    actionName = slashPos + 1;
                    result = EXECUTE_COMMAND_ERROR; /*this only exists to quench a compiler warning about returning an uninitialized variable, which is not possible by design*/
                }
            }
        }
    } while (actionName != NULL);
    return result;
}

static METHODRETURN_HANDLE ScanMethodPathAndExecuteMethod(COMMAND_DECODER_HANDLE_DATA* commandDecoderInstance, SCHEMA_HANDLE schemaHandle, const char* fullMethodName, MULTITREE_HANDLE methodTree)
{
    METHODRETURN_HANDLE result;
    char* relativeMethodPath;
    const char* methodName = fullMethodName;
    SCHEMA_MODEL_TYPE_HANDLE modelHandle = commandDecoderInstance->ModelHandle;

    /*Codes_SRS_COMMAND_DECODER_02_018: [ CommandDecoder_ExecuteMethod shall validate that consecutive segments of the fullMethodName exist in the model. ]*/
    /*Codes_SRS_COMMAND_DECODER_02_019: [ CommandDecoder_ExecuteMethod shall locate the final model to which the methodName applies. ]*/
    do
    {
        /* find the slash */
        const char* slashPos = strchr(methodName, '/');
        if (slashPos == NULL)
        {
            size_t relativeMethodPathLength;

            if (methodName == fullMethodName)
            {
                relativeMethodPathLength = 0;
            }
            else
            {
                relativeMethodPathLength = methodName - fullMethodName - 1;
            }

            relativeMethodPath = (char*)malloc(relativeMethodPathLength + 1);
            if (relativeMethodPath == NULL)
            {
                /*Codes_SRS_COMMAND_DECODER_02_023: [ If any of the previous operations fail, then CommandDecoder_ExecuteMethod shall return NULL. ]*/
                LogError("Failed allocating relative method path");
                result = NULL;
            }
            else
            {
                strncpy(relativeMethodPath, fullMethodName, relativeMethodPathLength);
                relativeMethodPath[relativeMethodPathLength] = 0;

                /* no slash found, this must be an method */
                result = DecodeAndExecuteModelMethod(commandDecoderInstance, schemaHandle, modelHandle, relativeMethodPath, methodName, methodTree);

                free(relativeMethodPath);
                methodName = NULL;
            }
            break;
        }
        else
        {
            /* found a slash, get the child model name */
            size_t modelLength = slashPos - methodName;
            char* childModelName = (char*)malloc(modelLength + 1);
            if (childModelName == NULL)
            {
                /*Codes_SRS_COMMAND_DECODER_02_023: [ If any of the previous operations fail, then CommandDecoder_ExecuteMethod shall return NULL. ]*/
                LogError("Failed allocating child model name");
                result = NULL;
                break;
            }
            else
            {
                strncpy(childModelName, methodName, modelLength);
                childModelName[modelLength] = 0;

                /* find the model */
                modelHandle = Schema_GetModelModelByName(modelHandle, childModelName);
                if (modelHandle == NULL)
                {
                    /*Codes_SRS_COMMAND_DECODER_02_023: [ If any of the previous operations fail, then CommandDecoder_ExecuteMethod shall return NULL. ]*/
                    LogError("Getting the model %s failed", childModelName);
                    free(childModelName);
                    result = NULL;
                    break;
                }
                else
                {
                    free(childModelName);
                    methodName = slashPos + 1;
                    result = NULL; /*this only exists to quench a compiler warning about returning an uninitialized variable, which is not possible by design*/
                }
            }
        }
    } while (methodName != NULL);
    return result;
}

static EXECUTE_COMMAND_RESULT DecodeCommand(COMMAND_DECODER_HANDLE_DATA* commandDecoderInstance, MULTITREE_HANDLE commandNode)
{
    EXECUTE_COMMAND_RESULT result;
    SCHEMA_HANDLE schemaHandle;

    /* Codes_SRS_COMMAND_DECODER_99_022:[ CommandDecoder shall use the Schema APIs to obtain the information about the entity set name and namespace] */
    if ((schemaHandle = Schema_GetSchemaForModelType(commandDecoderInstance->ModelHandle)) == NULL)
    {
        /* Codes_SRS_COMMAND_DECODER_99_010:[ If any Schema API fails then the command shall not be dispatched and it shall return EXECUTE_COMMAND_ERROR.]*/
        LogError("Getting schema information failed");
        result = EXECUTE_COMMAND_ERROR;
    }
    else
    {
        const char* actionName;
        MULTITREE_HANDLE nameTreeNode;

        /* Codes_SRS_COMMAND_DECODER_01_014: [CommandDecoder shall use the MultiTree APIs to extract a specific element from the command JSON.] */
        /* Codes_SRS_COMMAND_DECODER_99_006:[ The action name shall be decoded from the element "name" of the command JSON.] */
        if ((MultiTree_GetChildByName(commandNode, "Name", &nameTreeNode) != MULTITREE_OK) ||
            (MultiTree_GetValue(nameTreeNode, (const void **)&actionName) != MULTITREE_OK))
        {
            /* Codes_SRS_COMMAND_DECODER_01_015: [If any MultiTree API call fails then the processing shall stop and the command shall not be dispatched and it shall return EXECUTE_COMMAND_ERROR.] */
            LogError("Getting action name failed.");
            result = EXECUTE_COMMAND_ERROR;
        }
        else if (strlen(actionName) < 2)
        {
            /* Codes_SRS_COMMAND_DECODER_99_021:[ If the parsing of the command fails for any other reason the command shall not be dispatched.] */
            LogError("Invalid action name.");
            result = EXECUTE_COMMAND_ERROR;
        }
        else
        {
            actionName++;
            result = ScanActionPathAndExecuteAction(commandDecoderInstance, schemaHandle, actionName, commandNode);
        }
    }
    return result;
}

static METHODRETURN_HANDLE DecodeMethod(COMMAND_DECODER_HANDLE_DATA* commandDecoderInstance, const char* fullMethodName, MULTITREE_HANDLE methodTree)
{
    METHODRETURN_HANDLE result;
    SCHEMA_HANDLE schemaHandle;

    /*Codes_SRS_COMMAND_DECODER_02_017: [ CommandDecoder_ExecuteMethod shall get the SCHEMA_HANDLE associated with the modelHandle passed at CommandDecoder_Create. ]*/
    if ((schemaHandle = Schema_GetSchemaForModelType(commandDecoderInstance->ModelHandle)) == NULL)
    {
        LogError("Getting schema information failed");
        result = NULL;
    }
    else
    {
        result = ScanMethodPathAndExecuteMethod(commandDecoderInstance, schemaHandle, fullMethodName, methodTree);

    }
    return result;
}

/*Codes_SRS_COMMAND_DECODER_01_009: [Whenever CommandDecoder_ExecuteCommand is the command shall be decoded and further dispatched to the actionCallback passed in CommandDecoder_Create.]*/
EXECUTE_COMMAND_RESULT CommandDecoder_ExecuteCommand(COMMAND_DECODER_HANDLE handle, const char* command)
{
    EXECUTE_COMMAND_RESULT result;
    COMMAND_DECODER_HANDLE_DATA* commandDecoderInstance = (COMMAND_DECODER_HANDLE_DATA*)handle;
    /*Codes_SRS_COMMAND_DECODER_01_010: [If either the buffer or the receiveCallbackContext argument is NULL, the processing shall stop and the command shall not be dispatched and it shall return EXECUTE_COMMAND_ERROR.]*/
    if (
        (command == NULL) ||
        (commandDecoderInstance == NULL)
    )
    {
        LogError("Invalid argument, COMMAND_DECODER_HANDLE handle=%p, const char* command=%p", handle, command);
        result = EXECUTE_COMMAND_ERROR;
    }
    else
    {
        size_t size = strlen(command);
        char* commandJSON;

        /* Codes_SRS_COMMAND_DECODER_01_011: [If the size of the command is 0 then the processing shall stop and the command shall not be dispatched and it shall return EXECUTE_COMMAND_ERROR.]*/
        if (size == 0)
        {
            LogError("Failed because command size is zero");
            result = EXECUTE_COMMAND_ERROR;
        }
        /*Codes_SRS_COMMAND_DECODER_01_013: [If parsing the JSON to a multi tree fails, the processing shall stop and the command shall not be dispatched and it shall return EXECUTE_COMMAND_ERROR.]*/
        else if ((commandJSON = (char*)malloc(size + 1)) == NULL)
        {
            LogError("Failed to allocate temporary storage for the commands JSON");
            result = EXECUTE_COMMAND_ERROR;
        }
        else
        {
            MULTITREE_HANDLE commandsTree;

            (void)memcpy(commandJSON, command, size);
            commandJSON[size] = '\0';

            /* Codes_SRS_COMMAND_DECODER_01_012: [CommandDecoder shall decode the command JSON contained in buffer to a multi-tree by using JSONDecoder_JSON_To_MultiTree.] */
            if (JSONDecoder_JSON_To_MultiTree(commandJSON, &commandsTree) != JSON_DECODER_OK)
            {
                /* Codes_SRS_COMMAND_DECODER_01_013: [If parsing the JSON to a multi tree fails, the processing shall stop and the command shall not be dispatched and it shall return EXECUTE_COMMAND_ERROR.] */
                LogError("Decoding JSON to a multi tree failed");
                result = EXECUTE_COMMAND_ERROR;
            }
            else
            {
                result = DecodeCommand(commandDecoderInstance, commandsTree);

                /* Codes_SRS_COMMAND_DECODER_01_016: [CommandDecoder shall ensure that the multi-tree resulting from JSONDecoder_JSON_To_MultiTree is freed after the commands are executed.] */
                MultiTree_Destroy(commandsTree);
            }

            free(commandJSON);
        }
    }
    return result;
}

METHODRETURN_HANDLE CommandDecoder_ExecuteMethod(COMMAND_DECODER_HANDLE handle, const char* fullMethodName, const char* methodPayload)
{
    METHODRETURN_HANDLE result;
    /*Codes_SRS_COMMAND_DECODER_02_014: [ If handle is NULL then CommandDecoder_ExecuteMethod shall fail and return NULL. ]*/
    /*Codes_SRS_COMMAND_DECODER_02_015: [ If fulMethodName is NULL then CommandDecoder_ExecuteMethod shall fail and return NULL. ]*/
    if (
        (handle == NULL) ||
        (fullMethodName == NULL) /*methodPayload can be NULL*/
        )
    {
        LogError("Invalid argument, COMMAND_DECODER_HANDLE handle=%p, const char* fullMethodName=%p", handle, fullMethodName);
        result = NULL;
    }
    else
    {
        COMMAND_DECODER_HANDLE_DATA* commandDecoderInstance = (COMMAND_DECODER_HANDLE_DATA*)handle;
        /*Codes_SRS_COMMAND_DECODER_02_025: [ If methodCallback is NULL then CommandDecoder_ExecuteMethod shall fail and return NULL. ]*/
        if (commandDecoderInstance->methodCallback == NULL)
        {
            LogError("unable to execute a method when the methodCallback passed in CommandDecoder_Create is NULL");
            result = NULL;
        }
        else
        {
            /*Codes_SRS_COMMAND_DECODER_02_016: [ If methodPayload is not NULL then CommandDecoder_ExecuteMethod shall build a MULTITREE_HANDLE out of methodPayload. ]*/
            if (methodPayload == NULL)
            {
                result = DecodeMethod(commandDecoderInstance, fullMethodName, NULL);
            }
            else
            {
                char* methodJSON;

                if (mallocAndStrcpy_s(&methodJSON, methodPayload) != 0)
                {
                    LogError("Failed to allocate temporary storage for the method JSON");
                    result = NULL;
                }
                else
                {
                    MULTITREE_HANDLE methodTree;
                    if (JSONDecoder_JSON_To_MultiTree(methodJSON, &methodTree) != JSON_DECODER_OK)
                    {
                        LogError("Decoding JSON to a multi tree failed");
                        result = NULL;
                    }
                    else
                    {
                        result = DecodeMethod(commandDecoderInstance, fullMethodName, methodTree);
                        MultiTree_Destroy(methodTree);
                    }
                    free(methodJSON);
                }
            }
        }
    }
    return result;
}


COMMAND_DECODER_HANDLE CommandDecoder_Create(SCHEMA_MODEL_TYPE_HANDLE modelHandle, ACTION_CALLBACK_FUNC actionCallback, void* actionCallbackContext, METHOD_CALLBACK_FUNC methodCallback, void* methodCallbackContext)
{
    COMMAND_DECODER_HANDLE_DATA* result;
    /* Codes_SRS_COMMAND_DECODER_99_019:[ For all exposed APIs argument validity checks shall precede other checks.] */
    /* Codes_SRS_COMMAND_DECODER_01_003: [ If modelHandle is NULL, CommandDecoder_Create shall return NULL. ]*/
    if (modelHandle == NULL)
    {
        LogError("Invalid arguments: SCHEMA_MODEL_TYPE_HANDLE modelHandle=%p, ACTION_CALLBACK_FUNC actionCallback=%p, void* actionCallbackContext=%p, METHOD_CALLBACK_FUNC methodCallback=%p, void* methodCallbackContext=%p",
            modelHandle, actionCallback, actionCallbackContext, methodCallback, methodCallbackContext);
        result = NULL;
    }
    else
    {
        /* Codes_SRS_COMMAND_DECODER_01_001: [CommandDecoder_Create shall create a new instance of a CommandDecoder.] */
        result = malloc(sizeof(COMMAND_DECODER_HANDLE_DATA));
        if (result == NULL)
        {
            /* Codes_SRS_COMMAND_DECODER_01_004: [If any error is encountered during CommandDecoder_Create CommandDecoder_Create shall return NULL.] */
            /*return as is*/
        }
        else
        {
            result->ModelHandle = modelHandle;
            result->ActionCallback = actionCallback;
            result->ActionCallbackContext = actionCallbackContext;
            result->methodCallback = methodCallback;
            result->methodCallbackContext = methodCallbackContext;
        }
    }

    return result;
}

void CommandDecoder_Destroy(COMMAND_DECODER_HANDLE commandDecoderHandle)
{
    /* Codes_SRS_COMMAND_DECODER_01_007: [If CommandDecoder_Destroy is called with a NULL handle, CommandDecoder_Destroy shall do nothing.] */
    if (commandDecoderHandle != NULL)
    {
        COMMAND_DECODER_HANDLE_DATA* commandDecoderInstance = (COMMAND_DECODER_HANDLE_DATA*)commandDecoderHandle;

        /* Codes_SRS_COMMAND_DECODER_01_005: [CommandDecoder_Destroy shall free all resources associated with the commandDecoderHandle instance.] */
        free(commandDecoderInstance);
    }
}

MU_DEFINE_ENUM_STRINGS_WITHOUT_INVALID(AGENT_DATA_TYPE_TYPE, AGENT_DATA_TYPE_TYPE_VALUES);

/*validates that the multitree (coming from a JSON) is actually a serialization of the model (complete or incomplete)*/
/*if the serialization contains more than the model, then it fails.*/
/*if the serialization does not contain mandatory items from the model, it fails*/
static bool validateModel_vs_Multitree(void* startAddress, SCHEMA_MODEL_TYPE_HANDLE modelHandle, MULTITREE_HANDLE desiredPropertiesTree, size_t offset)
{

    bool result;
    size_t nChildren;
    size_t nProcessedChildren = 0;
    (void)MultiTree_GetChildCount(desiredPropertiesTree, &nChildren);
    for (size_t i = 0;i < nChildren;i++)
    {
        MULTITREE_HANDLE child;
        if (MultiTree_GetChild(desiredPropertiesTree, i, &child) != MULTITREE_OK)
        {
            LogError("failure in MultiTree_GetChild");
            i = nChildren;
        }
        else
        {
            STRING_HANDLE childName = STRING_new();
            if (childName == NULL)
            {
                LogError("failure to STRING_new");
                i = nChildren;
            }
            else
            {
                if (MultiTree_GetName(child, childName) != MULTITREE_OK)
                {
                    LogError("failure to MultiTree_GetName");
                    i = nChildren;
                }
                else
                {
                    const char *childName_str = STRING_c_str(childName);
                    SCHEMA_MODEL_ELEMENT elementType = Schema_GetModelElementByName(modelHandle, childName_str);
                    switch (elementType.elementType)
                    {
                        default:
                        {
                            LogError("INTERNAL ERROR: unexpected function return");
                            i = nChildren;
                            break;
                        }
                        case (SCHEMA_PROPERTY):
                        {
                            LogError("cannot ingest name (WITH_DATA instead of WITH_DESIRED_PROPERTY): %s", childName_str);
                            i = nChildren;
                            break;
                        }
                        case (SCHEMA_REPORTED_PROPERTY):
                        {
                            LogError("cannot ingest name (WITH_REPORTED_PROPERTY instead of WITH_DESIRED_PROPERTY): %s", childName_str);
                            i = nChildren;
                            break;
                        }
                        case (SCHEMA_DESIRED_PROPERTY):
                        {
                            /*Codes_SRS_COMMAND_DECODER_02_007: [ If the child name corresponds to a desired property then an AGENT_DATA_TYPE shall be constructed from the MULTITREE node. ]*/
                            SCHEMA_DESIRED_PROPERTY_HANDLE desiredPropertyHandle = elementType.elementHandle.desiredPropertyHandle;

                            const char* desiredPropertyType = Schema_GetModelDesiredPropertyType(desiredPropertyHandle);
                            AGENT_DATA_TYPE output;
                            if (DecodeValueFromNode(Schema_GetSchemaForModelType(modelHandle), &output, child, desiredPropertyType) != 0)
                            {
                                LogError("failure in DecodeValueFromNode");
                                i = nChildren;
                            }
                            else
                            {
                                /*Codes_SRS_COMMAND_DECODER_02_008: [ The desired property shall be constructed in memory by calling pfDesiredPropertyFromAGENT_DATA_TYPE. ]*/
                                pfDesiredPropertyFromAGENT_DATA_TYPE leFunction = Schema_GetModelDesiredProperty_pfDesiredPropertyFromAGENT_DATA_TYPE(desiredPropertyHandle);
                                if (leFunction(&output, (char*)startAddress + offset + Schema_GetModelDesiredProperty_offset(desiredPropertyHandle)) != 0)
                                {
                                    LogError("failure in a function that converts from AGENT_DATA_TYPE to C data");
                                }
                                else
                                {
                                    /*Codes_SRS_COMMAND_DECODER_02_013: [ If the desired property has a non-NULL pfOnDesiredProperty then it shall be called. ]*/
                                    pfOnDesiredProperty onDesiredProperty = Schema_GetModelDesiredProperty_pfOnDesiredProperty(desiredPropertyHandle);
                                    if (onDesiredProperty != NULL)
                                    {
                                        onDesiredProperty((char*)startAddress + offset);
                                    }
                                    nProcessedChildren++;
                                }
                                Destroy_AGENT_DATA_TYPE(&output);
                            }

                            break;
                        }
                        case(SCHEMA_MODEL_IN_MODEL):
                        {
                            SCHEMA_MODEL_TYPE_HANDLE modelModel = elementType.elementHandle.modelHandle;

                            /*Codes_SRS_COMMAND_DECODER_02_009: [ If the child name corresponds to a model in model then the function shall call itself recursively. ]*/
                            if (!validateModel_vs_Multitree(startAddress, modelModel, child, offset + Schema_GetModelModelByName_Offset(modelHandle, childName_str)))
                            {
                                LogError("failure in validateModel_vs_Multitree");
                                i = nChildren;
                            }
                            else
                            {
                                /*if the model in model so happened to be a WITH_DESIRED_PROPERTY... (only those has non_NULL pfOnDesiredProperty) */
                                /*Codes_SRS_COMMAND_DECODER_02_012: [ If the child model in model has a non-NULL pfOnDesiredProperty then pfOnDesiredProperty shall be called. ]*/
                                pfOnDesiredProperty onDesiredProperty = Schema_GetModelModelByName_OnDesiredProperty(modelHandle, childName_str);
                                if (onDesiredProperty != NULL)
                                {
                                    onDesiredProperty((char*)startAddress + offset);
                                }

                                nProcessedChildren++;
                            }

                            break;
                        }

                    } /*switch*/
                }
                STRING_delete(childName);
            }
        }
    }

    if(nProcessedChildren == nChildren)
    {
        /*Codes_SRS_COMMAND_DECODER_02_010: [ If the complete MULTITREE has been parsed then CommandDecoder_IngestDesiredProperties shall succeed and return EXECUTE_COMMAND_SUCCESS. ]*/
        result = true;
    }
    else
    {
        /*Codes_SRS_COMMAND_DECODER_02_011: [ Otherwise CommandDecoder_IngestDesiredProperties shall fail and return EXECUTE_COMMAND_FAILED. ]*/
        LogError("not all constituents of the JSON have been ingested");
        result = false;
    }
    return result;
}

static EXECUTE_COMMAND_RESULT DecodeDesiredProperties(void* startAddress, COMMAND_DECODER_HANDLE_DATA* handle, MULTITREE_HANDLE desiredPropertiesTree)
{
    /*Codes_SRS_COMMAND_DECODER_02_006: [ CommandDecoder_IngestDesiredProperties shall parse the MULTITREEE recursively. ]*/
    return validateModel_vs_Multitree(startAddress, handle->ModelHandle, desiredPropertiesTree, 0 )?EXECUTE_COMMAND_SUCCESS:EXECUTE_COMMAND_FAILED;
}

/* Raw JSON has properties we don't need; potentially nodes other than "desired" for full TWIN as well as a $version we don't pass to callees */
static bool RemoveUnneededTwinProperties(MULTITREE_HANDLE initialParsedTree, bool parseDesiredNode, MULTITREE_HANDLE *desiredPropertiesTree)
{
    MULTITREE_HANDLE updateTree;
    bool result;

    if (parseDesiredNode)
    {
        /*Codes_SRS_COMMAND_DECODER_02_014: [ If parseDesiredNode is TRUE, parse only the `desired` part of JSON tree ]*/
        if (MultiTree_GetChildByName(initialParsedTree, "desired", &updateTree) != MULTITREE_OK)
        {
            LogError("Unable to find 'desired' in tree");
            return false;
        }
    }
    else
    {
        // Tree already starts on node we want so just use it.
        updateTree = initialParsedTree;
    }

    /*Codes_COMMAND_DECODER_02_015: [ Remove '$version' string from node, if it is present.  It not being present is not an error ]*/
    MULTITREE_RESULT deleteChildResult = MultiTree_DeleteChild(updateTree, "$version");
    if ((deleteChildResult == MULTITREE_OK) || (deleteChildResult == MULTITREE_CHILD_NOT_FOUND))
    {
        *desiredPropertiesTree = updateTree;
        result = true;
    }
    else
    {
        *desiredPropertiesTree = NULL;
        result = false;
    }

    return result;
}

EXECUTE_COMMAND_RESULT CommandDecoder_IngestDesiredProperties(void* startAddress, COMMAND_DECODER_HANDLE handle, const char* jsonPayload, bool parseDesiredNode)
{
    EXECUTE_COMMAND_RESULT result;

    /*Codes_SRS_COMMAND_DECODER_02_001: [ If startAddress is NULL then CommandDecoder_IngestDesiredProperties shall fail and return EXECUTE_COMMAND_ERROR. ]*/
    /*Codes_SRS_COMMAND_DECODER_02_002: [ If handle is NULL then CommandDecoder_IngestDesiredProperties shall fail and return EXECUTE_COMMAND_ERROR. ]*/
    /*Codes_SRS_COMMAND_DECODER_02_003: [ If jsonPayload is NULL then CommandDecoder_IngestDesiredProperties shall fail and return EXECUTE_COMMAND_ERROR. ]*/
    if(
        (startAddress == NULL) ||
        (handle == NULL) ||
        (jsonPayload == NULL)
        )
    {
        LogError("invalid argument COMMAND_DECODER_HANDLE handle=%p, const char* jsonPayload=%p", handle, jsonPayload);
        result = EXECUTE_COMMAND_ERROR;
    }
    else
    {
        /*Codes_SRS_COMMAND_DECODER_02_004: [ CommandDecoder_IngestDesiredProperties shall clone desiredProperties. ]*/
        char* copy;
        if (mallocAndStrcpy_s(&copy, jsonPayload) != 0)
        {
            LogError("failure in mallocAndStrcpy_s");
            result = EXECUTE_COMMAND_FAILED;
        }
        else
        {
            /*Codes_SRS_COMMAND_DECODER_02_005: [ CommandDecoder_IngestDesiredProperties shall create a MULTITREE_HANDLE ouf of the clone of desiredProperties. ]*/
            MULTITREE_HANDLE initialParsedTree;
            MULTITREE_HANDLE desiredPropertiesTree;

            if (JSONDecoder_JSON_To_MultiTree(copy, &initialParsedTree) != JSON_DECODER_OK)
            {
                LogError("Decoding JSON to a multi tree failed");
                result = EXECUTE_COMMAND_ERROR;
            }
            else
            {
                if (RemoveUnneededTwinProperties(initialParsedTree, parseDesiredNode, &desiredPropertiesTree) == false)
                {
                    LogError("Removing unneeded twin properties failed");
                    result = EXECUTE_COMMAND_ERROR;
                }
                else
                {
                    COMMAND_DECODER_HANDLE_DATA* commandDecoderInstance = (COMMAND_DECODER_HANDLE_DATA*)handle;

                    /*Codes_SRS_COMMAND_DECODER_02_006: [ CommandDecoder_IngestDesiredProperties shall parse the MULTITREEE recursively. ]*/
                    result = DecodeDesiredProperties(startAddress, commandDecoderInstance, desiredPropertiesTree);

                    // Do NOT free desiredPropertiesTree.  It is only a pointer into initialParsedTree.
                    MultiTree_Destroy(initialParsedTree);
                }
            }
            free(copy);
        }
    }
    return result;
}
