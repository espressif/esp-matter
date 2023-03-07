// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "azure_c_shared_utility/gballoc.h"
#include "azure_c_shared_utility/crt_abstractions.h"

#include "jsondecoder.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stddef.h>

#define IsWhiteSpace(A) (((A) == 0x20) || ((A) == 0x09) || ((A) == 0x0A) || ((A) == 0x0D))

typedef struct PARSER_STATE_TAG
{
    char* json;
} PARSER_STATE;

static JSON_DECODER_RESULT ParseArray(PARSER_STATE* parserState, MULTITREE_HANDLE currentNode);
static JSON_DECODER_RESULT ParseObject(PARSER_STATE* parserState, MULTITREE_HANDLE currentNode);

/* Codes_SRS_JSON_DECODER_99_049:[ JSONDecoder shall not allocate new string values for the leafs, but rather point to strings in the original JSON.] */
static void NoFreeFunction(void* value)
{
    (void)value;
}

static int NOPCloneFunction(void** destination, const void* source)
{
    *destination = (void**)source;
    return 0;
}

void SkipWhiteSpaces(PARSER_STATE* parserState)
{
    while ((*(parserState->json) != '\0') && IsWhiteSpace(*(parserState->json)))
    {
        parserState->json++;
    }
}

static JSON_DECODER_RESULT ParseString(PARSER_STATE* parserState, char** stringBegin)
{
    JSON_DECODER_RESULT result = JSON_DECODER_OK;
    *stringBegin = parserState->json;

    /* Codes_SRS_JSON_DECODER_99_028:[ A string begins and ends with quotation marks.] */
    if (*(parserState->json) != '"')
    {
        /* Codes_SRS_JSON_DECODER_99_007:[ If parsing the JSON fails due to the JSON string being malformed, JSONDecoder_JSON_To_MultiTree shall return JSON_DECODER_PARSE_ERROR.] */
        result = JSON_DECODER_PARSE_ERROR;
    }
    else
    {
        parserState->json++;
        while ((*(parserState->json) != '"') && (*(parserState->json) != '\0'))
        {
            /* Codes_SRS_JSON_DECODER_99_030:[ Any character may be escaped.]  */
            /* Codes_SRS_JSON_DECODER_99_033:[ Alternatively, there are two-character sequence escape  representations of some popular characters.  So, for example, a string containing only a single reverse solidus character may be represented more compactly as "\\".] */
            if (*(parserState->json) == '\\')
            {
                parserState->json++;
                if (
                    /* Codes_SRS_JSON_DECODER_99_051:[ %x5C /          ; \    reverse solidus U+005C] */
                    (*parserState->json == '\\') ||
                    /* Codes_SRS_JSON_DECODER_99_050:[ %x22 /          ; "    quotation mark  U+0022] */
                    (*parserState->json == '"') ||
                    /* Codes_SRS_JSON_DECODER_99_052:[ %x2F /          ; /    solidus         U+002F] */
                    (*parserState->json == '/') ||
                    /* Codes_SRS_JSON_DECODER_99_053:[ %x62 /          ; b    backspace       U+0008] */
                    (*parserState->json == 'b') ||
                    /* Codes_SRS_JSON_DECODER_99_054:[ %x66 /          ; f    form feed       U+000C] */
                    (*parserState->json == 'f') ||
                    /* Codes_SRS_JSON_DECODER_99_055:[ %x6E /          ; n    line feed       U+000A] */
                    (*parserState->json == 'n') ||
                    /* Codes_SRS_JSON_DECODER_99_056:[ %x72 /          ; r    carriage return U+000D] */
                    (*parserState->json == 'r') ||
                    /* Codes_SRS_JSON_DECODER_99_057:[ %x74 /          ; t    tab             U+0009] */
                    (*parserState->json == 't'))
                {
                    parserState->json++;
                }
                else
                {
                    /* Codes_SRS_JSON_DECODER_99_007:[ If parsing the JSON fails due to the JSON string being malformed, JSONDecoder_JSON_To_MultiTree shall return JSON_DECODER_PARSE_ERROR.] */
                    result = JSON_DECODER_PARSE_ERROR;
                    break;
                }
            }
            else
            {
                parserState->json++;
            }
        }

        if (*(parserState->json) != '"')
        {
            /* Codes_SRS_JSON_DECODER_99_007:[ If parsing the JSON fails due to the JSON string being malformed, JSONDecoder_JSON_To_MultiTree shall return JSON_DECODER_PARSE_ERROR.] */
            result = JSON_DECODER_PARSE_ERROR;
        }
        else
        {
            parserState->json++;
            result = JSON_DECODER_OK;
        }
    }

    return result;
}

static JSON_DECODER_RESULT ParseNumber(PARSER_STATE* parserState)
{
    JSON_DECODER_RESULT result = JSON_DECODER_OK;
    size_t digitCount = 0;

    if (*(parserState->json) == '-')
    {
        parserState->json++;
    }

    /* Codes_SRS_JSON_DECODER_99_043:[ A number contains an integer component that may be prefixed with an optional minus sign, which may be followed by a fraction part and/or an exponent part.] */
    while (*(parserState->json) != '\0')
    {
        /* Codes_SRS_JSON_DECODER_99_044:[ Octal and hex forms are not allowed.] */
        if (ISDIGIT(*(parserState->json)))
        {
            digitCount++;
            /* simply continue */
        }
        else
        {
            break;
        }

        parserState->json++;
    }

    if ((digitCount == 0) ||
        /* Codes_SRS_JSON_DECODER_99_045:[ Leading zeros are not allowed.] */
        ((digitCount > 1) && *(parserState->json - digitCount) == '0'))
    {
        /* Codes_SRS_JSON_DECODER_99_007:[ If parsing the JSON fails due to the JSON string being malformed, JSONDecoder_JSON_To_MultiTree shall return JSON_DECODER_PARSE_ERROR.] */
        result = JSON_DECODER_PARSE_ERROR;
    }
    else
    {
        /* Codes_SRS_JSON_DECODER_99_046:[ A fraction part is a decimal point followed by one or more digits.] */
        if (*(parserState->json) == '.')
        {
            /* optional fractional part */
            parserState->json++;
            digitCount = 0;

            while (*(parserState->json) != '\0')
            {
                /* Codes_SRS_JSON_DECODER_99_044:[ Octal and hex forms are not allowed.] */
                if (ISDIGIT(*(parserState->json)))
                {
                    digitCount++;
                    /* simply continue */
                }
                else
                {
                    break;
                }

                parserState->json++;
            }

            if (digitCount == 0)
            {
                /* Codes_SRS_JSON_DECODER_99_007:[ If parsing the JSON fails due to the JSON string being malformed, JSONDecoder_JSON_To_MultiTree shall return JSON_DECODER_PARSE_ERROR.] */
                result = JSON_DECODER_PARSE_ERROR;
            }
        }

        /* Codes_SRS_JSON_DECODER_99_047:[ An exponent part begins with the letter E in upper or lowercase, which may be followed by a plus or minus sign.] */
        if ((*(parserState->json) == 'e') || (*(parserState->json) == 'E'))
        {
            parserState->json++;

            /* optional sign */
            if ((*(parserState->json) == '-') || (*(parserState->json) == '+'))
            {
                parserState->json++;
            }

            digitCount = 0;

            /* Codes_SRS_JSON_DECODER_99_048:[ The E and optional sign are followed by one or more digits.] */
            while (*(parserState->json) != '\0')
            {
                /* Codes_SRS_JSON_DECODER_99_044:[ Octal and hex forms are not allowed.] */
                if (ISDIGIT(*(parserState->json)))
                {
                    digitCount++;
                    /* simply continue */
                }
                else
                {
                    break;
                }

                parserState->json++;
            }

            if (digitCount == 0)
            {
                /* Codes_SRS_JSON_DECODER_99_007:[ If parsing the JSON fails due to the JSON string being malformed, JSONDecoder_JSON_To_MultiTree shall return JSON_DECODER_PARSE_ERROR.] */
                result = JSON_DECODER_PARSE_ERROR;
            }
        }
    }

    return result;
}

static JSON_DECODER_RESULT ParseValue(PARSER_STATE* parserState, MULTITREE_HANDLE currentNode, char** stringBegin)
{
    JSON_DECODER_RESULT result;

    SkipWhiteSpaces(parserState);

    if (*(parserState->json) == '"')
    {
        result = ParseString(parserState, stringBegin);
    }
    /* Codes_SRS_JSON_DECODER_99_018:[ A JSON value MUST be an object, array, number, or string, or one of the following three literal names: false null true] */
    /* Codes_SRS_JSON_DECODER_99_019:[ The literal names MUST be lowercase.] */
    /* Codes_SRS_JSON_DECODER_99_020:[ No other literal names are allowed.] */
    else if (strncmp(parserState->json, "false", 5) == 0)
    {
        *stringBegin = parserState->json;
        parserState->json += 5;
        result = JSON_DECODER_OK;
    }
    else if (strncmp(parserState->json, "true", 4) == 0)
    {
        *stringBegin = parserState->json;
        parserState->json += 4;
        result = JSON_DECODER_OK;
    }
    else if (strncmp(parserState->json, "null", 4) == 0)
    {
        *stringBegin = parserState->json;
        parserState->json += 4;
        result = JSON_DECODER_OK;
    }
    /* Tests_SRS_JSON_DECODER_99_018:[ A JSON value MUST be an object, array, number, or string, or one of the following three literal names: false null true] */
    else if (*(parserState->json) == '[')
    {
        result = ParseArray(parserState, currentNode);
        *stringBegin = NULL;
    }
    else if (*(parserState->json) == '{')
    {
        result = ParseObject(parserState, currentNode);
        *stringBegin = NULL;
    }
    else if (
        (
            ISDIGIT(*(parserState->json))
        )
        || (*(parserState->json) == '-'))
    {
        *stringBegin = parserState->json;
        result = ParseNumber(parserState);
    }
    else
    {
        /* Codes_SRS_JSON_DECODER_99_007:[ If parsing the JSON fails due to the JSON string being malformed, JSONDecoder_JSON_To_MultiTree shall return JSON_DECODER_PARSE_ERROR.] */
        result = JSON_DECODER_PARSE_ERROR;
    }

    return result;
}

static JSON_DECODER_RESULT ParseColon(PARSER_STATE* parserState)
{
    JSON_DECODER_RESULT result;

    SkipWhiteSpaces(parserState);
    /* Codes_SRS_JSON_DECODER_99_023:[  A single colon comes after each name, separating the name from the value.] */
    if (*(parserState->json) != ':')
    {
        /* Codes_SRS_JSON_DECODER_99_007:[ If parsing the JSON fails due to the JSON string being malformed, JSONDecoder_JSON_To_MultiTree shall return JSON_DECODER_PARSE_ERROR.] */
        result = JSON_DECODER_PARSE_ERROR;
    }
    else
    {
        parserState->json++;
        result = JSON_DECODER_OK;
    }

    return result;
}

static JSON_DECODER_RESULT ParseOpenCurly(PARSER_STATE* parserState)
{
    JSON_DECODER_RESULT result = JSON_DECODER_OK;

    SkipWhiteSpaces(parserState);

    /* Codes_SRS_JSON_DECODER_99_021:[    An object structure is represented as a pair of curly brackets surrounding zero or more name/value pairs (or members).] */
    if (*(parserState->json) != '{')
    {
        /* Codes_SRS_JSON_DECODER_99_007:[ If parsing the JSON fails due to the JSON string being malformed, JSONDecoder_JSON_To_MultiTree shall return JSON_DECODER_PARSE_ERROR.] */
        result = JSON_DECODER_PARSE_ERROR;
    }
    else
    {
        parserState->json++;
        result = JSON_DECODER_OK;
    }

    return result;
}

static JSON_DECODER_RESULT ParseNameValuePair(PARSER_STATE* parserState, MULTITREE_HANDLE currentNode)
{
    JSON_DECODER_RESULT result;
    char* memberNameBegin;

    SkipWhiteSpaces(parserState);

    /* Codes_SRS_JSON_DECODER_99_022:[ A name is a string.] */
    result = ParseString(parserState, &memberNameBegin);
    if (result == JSON_DECODER_OK)
    {
        char* valueBegin;
        MULTITREE_HANDLE childNode;
        *(parserState->json - 1) = 0;

        result = ParseColon(parserState);
        if (result == JSON_DECODER_OK)
        {
            /* Codes_SRS_JSON_DECODER_99_025:[ The names within an object SHOULD be unique.] */
            /* Multi Tree takes care of not having 2 children with the same name */
            /* Codes_SRS_JSON_DECODER_99_002:[ JSONDecoder_JSON_To_MultiTree shall use the MultiTree APIs to create the multi tree and add leafs to the multi tree.] */
            /* Codes_SRS_JSON_DECODER_99_003:[ When a JSON element is decoded from the JSON object then a leaf shall be added to the MultiTree.] */
            /* Codes_SRS_JSON_DECODER_99_004:[ The leaf node name in the multi tree shall be the JSON element name.] */
            if (MultiTree_AddChild(currentNode, memberNameBegin + 1, &childNode) != MULTITREE_OK)
            {
                /* Codes_SRS_JSON_DECODER_99_038:[ If any MultiTree API fails, JSONDecoder_JSON_To_MultiTree shall return JSON_DECODER_MULTITREE_FAILED.] */
                result = JSON_DECODER_MULTITREE_FAILED;
            }
            else
            {
                result = ParseValue(parserState, childNode, &valueBegin);
                if ((result == JSON_DECODER_OK) && (valueBegin != NULL))
                {
                    /* Codes_SRS_JSON_DECODER_99_005:[ The leaf node added in the multi tree shall have the value the string value of the JSON element as parsed from the JSON object.] */
                    if (MultiTree_SetValue(childNode, valueBegin) != MULTITREE_OK)
                    {
                        /* Codes_SRS_JSON_DECODER_99_038:[ If any MultiTree API fails, JSONDecoder_JSON_To_MultiTree shall return JSON_DECODER_MULTITREE_FAILED.] */
                        result = JSON_DECODER_MULTITREE_FAILED;
                    }
                }
            }
        }
    }

    return result;
}

static JSON_DECODER_RESULT ParseObject(PARSER_STATE* parserState, MULTITREE_HANDLE currentNode)
{
    JSON_DECODER_RESULT result = ParseOpenCurly(parserState);
    if (result == JSON_DECODER_OK)
    {
        char jsonChar;

        SkipWhiteSpaces(parserState);

        jsonChar = *(parserState->json);
        while ((jsonChar != '}') && (jsonChar != '\0'))
        {
            char* valueEnd;

            /* decode each value */
            result = ParseNameValuePair(parserState, currentNode);
            if (result != JSON_DECODER_OK)
            {
                break;
            }

            valueEnd = parserState->json;

            SkipWhiteSpaces(parserState);
            jsonChar = *(parserState->json);
            *valueEnd = 0;

            /* Codes_SRS_JSON_DECODER_99_024:[ A single comma separates a value from a following name.] */
            if (jsonChar == ',')
            {
                parserState->json++;
                /* get the next name/value pair */
            }
        }

        if (result != JSON_DECODER_OK)
        {
            /* already have error */
        }
        else
        {
            if (jsonChar != '}')
            {
                /* Codes_SRS_JSON_DECODER_99_007:[ If parsing the JSON fails due to the JSON string being malformed, JSONDecoder_JSON_To_MultiTree shall return JSON_DECODER_PARSE_ERROR.] */
                result = JSON_DECODER_PARSE_ERROR;
            }
            else
            {
                parserState->json++;
            }
        }
    }

    return result;
}

static JSON_DECODER_RESULT ParseArray(PARSER_STATE* parserState, MULTITREE_HANDLE currentNode)
{
    JSON_DECODER_RESULT result = JSON_DECODER_OK;

    SkipWhiteSpaces(parserState);

    /* Codes_SRS_JSON_DECODER_99_026:[ An array structure is represented as square brackets surrounding zero or more values (or elements).] */
    if (*(parserState->json) != '[')
    {
        /* Codes_SRS_JSON_DECODER_99_007:[ If parsing the JSON fails due to the JSON string being malformed, JSONDecoder_JSON_To_MultiTree shall return JSON_DECODER_PARSE_ERROR.] */
        result = JSON_DECODER_PARSE_ERROR;
    }
    else
    {
        char* stringBegin;
        char jsonChar;
        int arrayIndex = 0;
        result = JSON_DECODER_OK;

        parserState->json++;

        SkipWhiteSpaces(parserState);

        jsonChar = *parserState->json;
        while ((jsonChar != ']') && (jsonChar != '\0'))
        {
            char arrayIndexStr[22];
            MULTITREE_HANDLE childNode;

            /* Codes_SRS_JSON_DECODER_99_039:[ For array elements the multi tree node name shall be the string representation of the array index.] */
            if (sprintf(arrayIndexStr, "%d", arrayIndex++) < 0)
            {
                result = JSON_DECODER_ERROR;
                break;
            }
            /* Codes_SRS_JSON_DECODER_99_003:[ When a JSON element is decoded from the JSON object then a leaf shall be added to the MultiTree.] */
            else if (MultiTree_AddChild(currentNode, arrayIndexStr, &childNode) != MULTITREE_OK)
            {
                /* Codes_SRS_JSON_DECODER_99_038:[ If any MultiTree API fails, JSONDecoder_JSON_To_MultiTree shall return JSON_DECODER_MULTITREE_FAILED.] */
                result = JSON_DECODER_MULTITREE_FAILED;
            }
            else
            {
                char* valueEnd;

                /* decode each value */
                result = ParseValue(parserState, childNode, &stringBegin);
                if (result != JSON_DECODER_OK)
                {
                    break;
                }

                if (stringBegin != NULL)
                {
                    /* Codes_SRS_JSON_DECODER_99_005:[ The leaf node added in the multi tree shall have the value the string value of the JSON element as parsed from the JSON object.] */
                    if (MultiTree_SetValue(childNode, stringBegin) != MULTITREE_OK)
                    {
                        /* Codes_SRS_JSON_DECODER_99_038:[ If any MultiTree API fails, JSONDecoder_JSON_To_MultiTree shall return JSON_DECODER_MULTITREE_FAILED.] */
                        result = JSON_DECODER_MULTITREE_FAILED;
                        break;
                    }
                }

                valueEnd = parserState->json;

                SkipWhiteSpaces(parserState);
                jsonChar = *(parserState->json);
                *valueEnd = 0;

                /* Codes_SRS_JSON_DECODER_99_027:[ Elements are separated by commas.] */
                if (jsonChar == ',')
                {
                    parserState->json++;
                    /* get the next value pair */
                }
                else if (jsonChar == ']')
                {
                    break;
                }
                else
                {
                    /* Codes_SRS_JSON_DECODER_99_007:[ If parsing the JSON fails due to the JSON string being malformed, JSONDecoder_JSON_To_MultiTree shall return JSON_DECODER_PARSE_ERROR.] */
                    result = JSON_DECODER_PARSE_ERROR;
                    break;
                }
            }
        }

        if (result != JSON_DECODER_OK)
        {
            /* already have error */
        }
        else
        {
            if (jsonChar != ']')
            {
                /* Codes_SRS_JSON_DECODER_99_007:[ If parsing the JSON fails due to the JSON string being malformed, JSONDecoder_JSON_To_MultiTree shall return JSON_DECODER_PARSE_ERROR.] */
                result = JSON_DECODER_PARSE_ERROR;
            }
            else
            {
                parserState->json++;
                SkipWhiteSpaces(parserState);
            }
        }
    }

    return result;
}

/* Codes_SRS_JSON_DECODER_99_012:[ A JSON text is a serialized object or array.] */
static JSON_DECODER_RESULT ParseObjectOrArray(PARSER_STATE* parserState, MULTITREE_HANDLE currentNode)
{
    JSON_DECODER_RESULT result = JSON_DECODER_PARSE_ERROR;

    SkipWhiteSpaces(parserState);

    if (*(parserState->json) == '{')
    {
        result = ParseObject(parserState, currentNode);
        SkipWhiteSpaces(parserState);
    }
    else if (*(parserState->json) == '[')
    {
        result = ParseArray(parserState, currentNode);
        SkipWhiteSpaces(parserState);
    }
    else
    {
        /* Codes_SRS_JSON_DECODER_99_007:[ If parsing the JSON fails due to the JSON string being malformed, JSONDecoder_JSON_To_MultiTree shall return JSON_DECODER_PARSE_ERROR.] */
        result = JSON_DECODER_PARSE_ERROR;
    }

    if ((result == JSON_DECODER_OK) &&
        (*(parserState->json) != '\0'))
    {
        /* Codes_SRS_JSON_DECODER_99_007:[ If parsing the JSON fails due to the JSON string being malformed, JSONDecoder_JSON_To_MultiTree shall return JSON_DECODER_PARSE_ERROR.] */
        result = JSON_DECODER_PARSE_ERROR;
    }

    return result;
}

static JSON_DECODER_RESULT ParseJSON(char* json, MULTITREE_HANDLE currentNode)
{
    /* Codes_SRS_JSON_DECODER_99_009:[ On success, JSONDecoder_JSON_To_MultiTree shall return a handle to the multi tree it created in the multiTreeHandle argument and it shall return JSON_DECODER_OK.] */
    PARSER_STATE parseState;
    parseState.json = json;
    return ParseObjectOrArray(&parseState, currentNode);
}

JSON_DECODER_RESULT JSONDecoder_JSON_To_MultiTree(char* json, MULTITREE_HANDLE* multiTreeHandle)
{
    JSON_DECODER_RESULT result;

    if ((json == NULL) ||
        (multiTreeHandle == NULL))
    {
        /* Codes_SRS_JSON_DECODER_99_001:[ If any of the parameters passed to the JSONDecoder_JSON_To_MultiTree function is NULL then the function call shall return JSON_DECODER_INVALID_ARG.] */
        result = JSON_DECODER_INVALID_ARG;
    }
    else if (*json == '\0')
    {
        /* Codes_SRS_JSON_DECODER_99_007:[ If parsing the JSON fails due to the JSON string being malformed, JSONDecoder_JSON_To_MultiTree shall return JSON_DECODER_PARSE_ERROR.] */
        result = JSON_DECODER_PARSE_ERROR;
    }
    else
    {
        /* Codes_SRS_JSON_DECODER_99_008:[ JSONDecoder_JSON_To_MultiTree shall create a multi tree based on the json string argument.] */
        /* Codes_SRS_JSON_DECODER_99_002:[ JSONDecoder_JSON_To_MultiTree shall use the MultiTree APIs to create the multi tree and add leafs to the multi tree.] */
        /* Codes_SRS_JSON_DECODER_99_009:[ On success, JSONDecoder_JSON_To_MultiTree shall return a handle to the multi tree it created in the multiTreeHandle argument and it shall return JSON_DECODER_OK.] */
        *multiTreeHandle = MultiTree_Create(NOPCloneFunction, NoFreeFunction);
        if (*multiTreeHandle == NULL)
        {
            /* Codes_SRS_JSON_DECODER_99_038:[ If any MultiTree API fails, JSONDecoder_JSON_To_MultiTree shall return JSON_DECODER_MULTITREE_FAILED.] */
            result = JSON_DECODER_MULTITREE_FAILED;
        }
        else
        {
            result = ParseJSON(json, *multiTreeHandle);
            if (result != JSON_DECODER_OK)
            {
                MultiTree_Destroy(*multiTreeHandle);
            }
        }
    }

    return result;
}
