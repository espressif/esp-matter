// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>
#include <stdio.h>
#include "azure_c_shared_utility/gballoc.h"
#include "azure_c_shared_utility/uuid.h"
#include "azure_c_shared_utility/uniqueid.h"
#include "azure_c_shared_utility/optimize_size.h"
#include "azure_c_shared_utility/xlogging.h"

#define UUID_STRING_LENGTH          36
#define UUID_STRING_SIZE            (UUID_STRING_LENGTH + 1)
#define __SUCCESS__                 0
#define UUID_FORMAT_STRING          "%02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x-%02x%02x%02x%02x%02x%02x"

int UUID_from_string(const char* uuid_string, UUID_T* uuid)
{
    int result;

    // Codes_SRS_UUID_09_007: [ If uuid_string or uuid are NULL, UUID_from_string shall return a non-zero value ]
    if (uuid_string == NULL || uuid == NULL)
    {
        LogError("Invalid argument (uuid_string=%p, uuid=%p)", uuid_string, uuid);
        result = MU_FAILURE;
    }
    else
    {
        size_t uuid_string_length = strlen(uuid_string);

        if (uuid_string_length != UUID_STRING_LENGTH)
        {
            LogError("Unexpected size for an UUID string (%lu)", (unsigned long)uuid_string_length);
            result = MU_FAILURE;
        }
        else
        {
            // Codes_SRS_UUID_09_008: [ Each pair of digits in uuid_string, excluding dashes, shall be read as a single HEX value and saved on the respective position in uuid ]
            size_t i, j;
            char* uuid_bytes;

            uuid_bytes = (char *)uuid;
            // Codes_SRS_UUID_09_010: [ If no failures occur, UUID_from_string shall return zero ]
            result = __SUCCESS__;

            for (i = 0, j = 0; i < uuid_string_length; )
            {
                if (uuid_string[i] == '-')
                {
                    i++;
                }
                else
                {
                    char double_hex_digit[3] = { 0, 0, 0 };

                    (void)memcpy(double_hex_digit, uuid_string + i, 2);

                    if (sscanf(double_hex_digit, "%02hhx", uuid_bytes + j) != 1)
                    {
                        // Codes_SRS_UUID_09_009: [ If uuid fails to be generated, UUID_from_string shall return a non-zero value ]
                        LogError("Failed decoding UUID string (%lu)", (unsigned long)i);
                        result = MU_FAILURE;
                        break;
                    }
                    else
                    {
                        i += 2;
                        j++;
                    }
                }
            }
        }
    }

    return result;
}

char* UUID_to_string(const UUID_T* uuid)
{
    char* result;

    // Codes_SRS_UUID_09_011: [ If uuid is NULL, UUID_to_string shall return a non-zero value ]
    if (uuid == NULL)
    {
        LogError("Invalid argument (uuid is NULL)");
        result = NULL;
    }
    // Codes_SRS_UUID_09_012: [ UUID_to_string shall allocate a valid UUID string (uuid_string) as per RFC 4122 ]
    else if ((result = (char*)malloc(sizeof(char) * UUID_STRING_SIZE)) == NULL)
    {
        // Codes_SRS_UUID_09_013: [ If uuid_string fails to be allocated, UUID_to_string shall return NULL ]
        LogError("Failed allocating UUID string");
    }
    else
    {
        unsigned char* uuid_bytes;
        int number_of_chars_written;

        uuid_bytes = (unsigned char*)uuid;

        // Codes_SRS_UUID_09_014: [ Each character in uuid shall be written in the respective positions of uuid_string as a 2-digit HEX value ]  
        number_of_chars_written = sprintf(result, "%" PRI_UUID,
            UUID_FORMAT_VALUES(uuid_bytes));

        if (number_of_chars_written != UUID_STRING_LENGTH)
        {
            // Tests_SRS_UUID_09_015: [ If uuid_string fails to be set, UUID_to_string shall return NULL ]
            LogError("Failed encoding UUID string");
            free(result);
            result = NULL;
        }
    }

    // Codes_SRS_UUID_09_016: [ If no failures occur, UUID_to_string shall return uuid_string ]
    return result;
}

int UUID_generate(UUID_T* uuid)
{
    int result;

    // Codes_SRS_UUID_09_001: [ If uuid is NULL, UUID_generate shall return a non-zero value ]
    if (uuid == NULL)
    {
        LogError("Invalid argument (uuid is NULL)");
        result = MU_FAILURE;
    }
    else
    {
        char* uuid_string;

        if ((uuid_string = (char*)malloc(sizeof(char) * UUID_STRING_SIZE)) == NULL)
        {
            // Codes_SRS_UUID_09_003: [ If the UUID string fails to be obtained, UUID_generate shall fail and return a non-zero value ]
            LogError("Failed allocating UUID string");
            result = MU_FAILURE;
        }
        else
        {
            (void)memset(uuid_string, 0, sizeof(char) * UUID_STRING_SIZE);

            // Codes_SRS_UUID_09_002: [ UUID_generate shall obtain an UUID string from UniqueId_Generate ]
            if (UniqueId_Generate(uuid_string, UUID_STRING_SIZE) != UNIQUEID_OK)
            {
                // Codes_SRS_UUID_09_003: [ If the UUID string fails to be obtained, UUID_generate shall fail and return a non-zero value ]
                LogError("Failed generating UUID");
                result = MU_FAILURE;
            }
            // Codes_SRS_UUID_09_004: [ The UUID string shall be parsed into an UUID_T type (16 unsigned char array) and filled in uuid ]
            else if (UUID_from_string(uuid_string, uuid) != 0)
            {
                // Codes_SRS_UUID_09_005: [ If uuid fails to be set, UUID_generate shall fail and return a non-zero value ]
                LogError("Failed parsing UUID string");
                result = MU_FAILURE;
            }
            else
            {
                // Codes_SRS_UUID_09_006: [ If no failures occur, UUID_generate shall return zero ]
                result = __SUCCESS__;
            }

            free(uuid_string);
        }
    }

    return result;
}
