// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdbool.h>
#include "azure_c_shared_utility/connection_string_parser.h"
#include "azure_c_shared_utility/map.h"
#include "azure_c_shared_utility/strings.h"
#include "azure_c_shared_utility/string_tokenizer.h"
#include "azure_c_shared_utility/optimize_size.h"
#include "azure_c_shared_utility/xlogging.h"


MAP_HANDLE connectionstringparser_parse_from_char(const char* connection_string)
{
    MAP_HANDLE result;
    STRING_HANDLE connString = NULL;

    /* Codes_SRS_CONNECTIONSTRINGPARSER_21_020: [connectionstringparser_parse_from_char shall create a STRING_HANDLE from the connection_string passed in as argument and parse it using the connectionstringparser_parse.]*/
    if ((connString = STRING_construct(connection_string)) == NULL)
    {
        /* Codes_SRS_CONNECTIONSTRINGPARSER_21_021: [If connectionstringparser_parse_from_char get error creating a STRING_HANDLE, it shall return NULL.]*/
        LogError("Error constructing connection String");
        result = NULL;
    }
    else
    {
        result = connectionstringparser_parse(connString);
        STRING_delete(connString);
    }

    return result;
}

/* Codes_SRS_CONNECTIONSTRINGPARSER_01_001: [connectionstringparser_parse shall parse all key value pairs from the connection_string passed in as argument and return a new map that holds the key/value pairs.] */
MAP_HANDLE connectionstringparser_parse(STRING_HANDLE connection_string)
{
    MAP_HANDLE result;

    if (connection_string == NULL)
    {
        /* Codes_SRS_CONNECTIONSTRINGPARSER_01_002: [If connection_string is NULL then connectionstringparser_parse shall fail and return NULL.] */
        result = NULL;
        LogError("NULL connection string passed to tokenizer.");
    }
    else
    {
        /* Codes_SRS_CONNECTIONSTRINGPARSER_01_003: [connectionstringparser_parse shall create a STRING tokenizer to be used for parsing the connection string, by calling STRING_TOKENIZER_create.] */
        /* Codes_SRS_CONNECTIONSTRINGPARSER_01_004: [connectionstringparser_parse shall start scanning at the beginning of the connection string.] */
        STRING_TOKENIZER_HANDLE tokenizer = STRING_TOKENIZER_create(connection_string);
        if (tokenizer == NULL)
        {
            /* Codes_SRS_CONNECTIONSTRINGPARSER_01_015: [If STRING_TOKENIZER_create fails, connectionstringparser_parse shall fail and return NULL.] */
            result = NULL;
            LogError("Error creating STRING tokenizer.");
        }
        else
        {
            /* Codes_SRS_CONNECTIONSTRINGPARSER_01_016: [2 STRINGs shall be allocated in order to hold the to be parsed key and value tokens.] */
            STRING_HANDLE token_key_string = STRING_new();
            if (token_key_string == NULL)
            {
                /* Codes_SRS_CONNECTIONSTRINGPARSER_01_017: [If allocating the STRINGs fails connectionstringparser_parse shall fail and return NULL.] */
                result = NULL;
                LogError("Error creating key token STRING.");
            }
            else
            {
                STRING_HANDLE token_value_string = STRING_new();
                if (token_value_string == NULL)
                {
                    /* Codes_SRS_CONNECTIONSTRINGPARSER_01_017: [If allocating the STRINGs fails connectionstringparser_parse shall fail and return NULL.] */
                    result = NULL;
                    LogError("Error creating value token STRING.");
                }
                else
                {
                    result = Map_Create(NULL);
                    if (result == NULL)
                    {
                        /* Codes_SRS_CONNECTIONSTRINGPARSER_01_018: [If creating the result map fails, then connectionstringparser_parse shall return NULL.] */
                        LogError("Error creating Map.");
                    }
                    else
                    {
                        /* Codes_SRS_CONNECTIONSTRINGPARSER_01_005: [The following actions shall be repeated until parsing is complete:] */
                        /* Codes_SRS_CONNECTIONSTRINGPARSER_01_006: [connectionstringparser_parse shall find a token (the key of the key/value pair) delimited by the = character, by calling STRING_TOKENIZER_get_next_token.] */
                        /* Codes_SRS_CONNECTIONSTRINGPARSER_01_007: [If STRING_TOKENIZER_get_next_token fails, parsing shall be considered complete.] */
                        while (STRING_TOKENIZER_get_next_token(tokenizer, token_key_string, "=") == 0)
                        {
                            bool is_error = false;

                            /* Codes_SRS_CONNECTIONSTRINGPARSER_01_008: [connectionstringparser_parse shall find a token (the value of the key/value pair) delimited by the ; character, by calling STRING_TOKENIZER_get_next_token.] */
                            if (STRING_TOKENIZER_get_next_token(tokenizer, token_value_string, ";") != 0)
                            {
                                /* Codes_SRS_CONNECTIONSTRINGPARSER_01_009: [If STRING_TOKENIZER_get_next_token fails, connectionstringparser_parse shall fail and return NULL (freeing the allocated result map).] */
                                is_error = true;
                                LogError("Error reading value token from the connection string.");
                            }
                            else
                            {
                                /* Codes_SRS_CONNECTIONSTRINGPARSER_01_011: [The C strings for the key and value shall be extracted from the previously parsed STRINGs by using STRING_c_str.] */
                                const char* token = STRING_c_str(token_key_string);
                                /* Codes_SRS_CONNECTIONSTRINGPARSER_01_013: [If STRING_c_str fails then connectionstringparser_parse shall fail and return NULL (freeing the allocated result map).] */
                                if ((token == NULL) ||
                                    /* Codes_SRS_CONNECTIONSTRINGPARSER_01_019: [If the key length is zero then connectionstringparser_parse shall fail and return NULL (freeing the allocated result map).] */
                                    (strlen(token) == 0))
                                {
                                    is_error = true;
                                    LogError("The key token is NULL or empty.");
                                }
                                else
                                {
                                    /* Codes_SRS_CONNECTIONSTRINGPARSER_01_011: [The C strings for the key and value shall be extracted from the previously parsed STRINGs by using STRING_c_str.] */
                                    const char* value = STRING_c_str(token_value_string);
                                    if (value == NULL)
                                    {
                                        /* Codes_SRS_CONNECTIONSTRINGPARSER_01_013: [If STRING_c_str fails then connectionstringparser_parse shall fail and return NULL (freeing the allocated result map).] */
                                        is_error = true;
                                        LogError("Could not get C string for value token.");
                                    }
                                    else
                                    {
                                        /* Codes_SRS_CONNECTIONSTRINGPARSER_01_010: [The key and value shall be added to the result map by using Map_Add.] */
                                        if (Map_Add(result, token, value) != MAP_OK)
                                        {
                                            /* Codes_SRS_CONNECTIONSTRINGPARSER_01_012: [If Map_Add fails connectionstringparser_parse shall fail and return NULL (freeing the allocated result map).] */
                                            is_error = true;
                                            LogError("Could not add the key/value pair to the result map.");
                                        }
                                    }
                                }
                            }

                            if (is_error)
                            {
                                LogError("Error parsing connection string.");
                                Map_Destroy(result);
                                result = NULL;
                                break;
                            }
                        }
                    }

                    STRING_delete(token_value_string);
                }

                STRING_delete(token_key_string);
            }

            /* Codes_SRS_CONNECTIONSTRINGPARSER_01_014: [After the parsing is complete the previously allocated STRINGs and STRING tokenizer shall be freed by calling STRING_TOKENIZER_destroy.] */
            STRING_TOKENIZER_destroy(tokenizer);
        }
    }

    return result;
}

/* Codes_SRS_CONNECTIONSTRINGPARSER_21_022: [connectionstringparser_splitHostName_from_char shall split the provided hostName in name and suffix.]*/
int connectionstringparser_splitHostName_from_char(const char* hostName, STRING_HANDLE nameString, STRING_HANDLE suffixString)
{
    int result;
    const char* runHostName = hostName;

    if ((hostName == NULL) || ((*hostName) == '\0') || ((*hostName) == '.') || (nameString == NULL) || (suffixString == NULL))
    {
        /* Codes_SRS_CONNECTIONSTRINGPARSER_21_026: [If the hostName is NULL, connectionstringparser_splitHostName_from_char shall return MU_FAILURE.]*/
        /* Codes_SRS_CONNECTIONSTRINGPARSER_21_027: [If the hostName is an empty string, connectionstringparser_splitHostName_from_char shall return MU_FAILURE.]*/
        /* Codes_SRS_CONNECTIONSTRINGPARSER_21_028: [If the nameString is NULL, connectionstringparser_splitHostName_from_char shall return MU_FAILURE.]*/
        /* Codes_SRS_CONNECTIONSTRINGPARSER_21_029: [If the suffixString is NULL, connectionstringparser_splitHostName_from_char shall return MU_FAILURE.]*/
        /* Codes_SRS_CONNECTIONSTRINGPARSER_21_030: [If the hostName is not a valid host name, connectionstringparser_splitHostName_from_char shall return MU_FAILURE.]*/
        result = MU_FAILURE;
    }
    else
    {
        while ((*runHostName) != '\0')
        {
            if ((*runHostName) == '.')
            {
                /* Codes_SRS_CONNECTIONSTRINGPARSER_21_023: [connectionstringparser_splitHostName_from_char shall copy all characters, from the beginning of the hostName to the first . to the nameString.]*/
                /* Codes_SRS_CONNECTIONSTRINGPARSER_21_024: [connectionstringparser_splitHostName_from_char shall copy all characters, from the first . to the end of the hostName, to the suffixString.]*/
                runHostName++;
                break;
            }
            runHostName++;
        }

        if ((*runHostName) == '\0')
        {
            /* Codes_SRS_CONNECTIONSTRINGPARSER_21_030: [If the hostName is not a valid host name, connectionstringparser_splitHostName_from_char shall return MU_FAILURE.]*/
            result = MU_FAILURE;
        }
        else
        {
            /* Codes_SRS_CONNECTIONSTRINGPARSER_21_023: [connectionstringparser_splitHostName_from_char shall copy all characters, from the beginning of the hostName to the first . to the nameString.]*/
            if (STRING_copy_n(nameString, hostName, runHostName - hostName - 1) != 0)
            {
                /* Codes_SRS_CONNECTIONSTRINGPARSER_21_031: [If connectionstringparser_splitHostName_from_char get error copying the name to the nameString, it shall return MU_FAILURE.]*/
                result = MU_FAILURE;
            }
            /* Codes_SRS_CONNECTIONSTRINGPARSER_21_024: [connectionstringparser_splitHostName_from_char shall copy all characters, from the first . to the end of the hostName, to the suffixString.]*/
            else if (STRING_copy(suffixString, runHostName) != 0)
            {
                /* Codes_SRS_CONNECTIONSTRINGPARSER_21_032: [If connectionstringparser_splitHostName_from_char get error copying the suffix to the suffixString, it shall return MU_FAILURE.]*/
                result = MU_FAILURE;
            }
            else
            {
                /* Codes_SRS_CONNECTIONSTRINGPARSER_21_025: [If connectionstringparser_splitHostName_from_char get success splitting the hostName, it shall return 0.]*/
                result = 0;
            }
        }
    }

    return result;
}


int connectionstringparser_splitHostName(STRING_HANDLE hostNameString, STRING_HANDLE nameString, STRING_HANDLE suffixString)
{
    int result;

    if (hostNameString == NULL)
    {
        /* Codes_SRS_CONNECTIONSTRINGPARSER_21_034: [If the hostNameString is NULL, connectionstringparser_splitHostName shall return MU_FAILURE.]*/
        result = MU_FAILURE;
    }
    else
    {
        /* Codes_SRS_CONNECTIONSTRINGPARSER_21_033: [connectionstringparser_splitHostName shall convert the hostNameString to a connection_string passed in as argument, and call connectionstringparser_splitHostName_from_char.]*/
        result = connectionstringparser_splitHostName_from_char(STRING_c_str(hostNameString), nameString, suffixString);
    }

    return result;
}
