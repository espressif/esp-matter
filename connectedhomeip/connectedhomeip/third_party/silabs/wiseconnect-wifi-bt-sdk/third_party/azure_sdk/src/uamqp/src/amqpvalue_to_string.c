// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include "azure_macro_utils/macro_utils.h"
#include "azure_c_shared_utility/gballoc.h"
#include "azure_c_shared_utility/xlogging.h"
#include "azure_c_shared_utility/uuid.h"
#include "azure_uamqp_c/amqpvalue_to_string.h"
#include "azure_uamqp_c/amqpvalue.h"

#if _WIN32
/* The MS runtime does not have snprintf */
#define snprintf _snprintf
#endif

static int string_concat(char** string, const char* to_concat)
{
    int result;
    char* new_string;
    size_t length = strlen(to_concat) + 1;
    size_t src_length;

    if (*string != NULL)
    {
        src_length = strlen(*string);
    }
    else
    {
        src_length = 0;
    }

    new_string = (char*)realloc(*string, src_length + length);
    if (new_string == NULL)
    {
        LogError("Cannot allocate memory for the new string");
        result = MU_FAILURE;
    }
    else
    {
        *string = new_string;
        (void)memcpy(*string + src_length, to_concat, length);
        result = 0;
    }

    return result;
}

char* amqpvalue_to_string(AMQP_VALUE amqp_value)
{
    char* result = NULL;

    if (amqp_value != NULL)
    {
        AMQP_TYPE amqp_type = amqpvalue_get_type(amqp_value);
        switch (amqp_type)
        {
        default:
            LogError("Unknown AMQP type");
            result = NULL;
            break;

        case AMQP_TYPE_NULL:
            if (string_concat(&result, "NULL") != 0)
            {
                LogError("Failure building amqp value string");
                free(result);
                result = NULL;
            }
            break;
        case AMQP_TYPE_BOOL:
        {
            bool value;
            if (amqpvalue_get_boolean(amqp_value, &value) != 0)
            {
                LogError("Failure getting bool value");
                free(result);
                result = NULL;
            }
            else if (string_concat(&result, (value == true) ? "true" : "false") != 0)
            {
                LogError("Failure building amqp value string");
                free(result);
                result = NULL;
            }
            break;
        }
        case AMQP_TYPE_UBYTE:
        {
            char str_value[4];
            uint8_t value;
            if (amqpvalue_get_ubyte(amqp_value, &value) != 0)
            {
                LogError("Failure getting ubyte value");
                free(result);
                result = NULL;
            }
            else if ((sprintf(str_value, "%" PRIu8, value) < 0) ||
                     (string_concat(&result, str_value) != 0))
            {
                LogError("Failure building amqp value string");
                free(result);
                result = NULL;
            }
            break;
        }
        case AMQP_TYPE_USHORT:
        {
            char str_value[6];
            uint16_t value;
            if (amqpvalue_get_ushort(amqp_value, &value) != 0)
            {
                LogError("Failure getting ushort value");
                free(result);
                result = NULL;
            }
            else if ((sprintf(str_value, "%" PRIu16, value) < 0) ||
                     (string_concat(&result, str_value) != 0))
            {
                LogError("Failure building amqp value string");
                free(result);
                result = NULL;
            }
            break;
        }
        case AMQP_TYPE_UINT:
        {
            char str_value[11];
            uint32_t value;
            if (amqpvalue_get_uint(amqp_value, &value) != 0)
            {
                LogError("Failure getting uint value");
                free(result);
                result = NULL;
            }
            else if ((sprintf(str_value, "%" PRIu32, value) < 0) ||
                     (string_concat(&result, str_value) != 0))
            {
                LogError("Failure building amqp value string");
                free(result);
                result = NULL;
            }
            break;
        }
        case AMQP_TYPE_ULONG:
        {
            char str_value[21];
            uint64_t value;
            if (amqpvalue_get_ulong(amqp_value, &value) != 0)
            {
                LogError("Failure getting ulong value");
                free(result);
                result = NULL;
            }
            else if ((sprintf(str_value, "%" PRIu64, value) < 0) ||
                     (string_concat(&result, str_value) != 0))
            {
                LogError("Failure building amqp value string");
                free(result);
                result = NULL;
            }
            break;
        }
        case AMQP_TYPE_BYTE:
        {
            char str_value[5];
            char value;
            if (amqpvalue_get_byte(amqp_value, &value) != 0)
            {
                LogError("Failure getting byte value");
                free(result);
                result = NULL;
            }
            else if ((sprintf(str_value, "%" PRId8, value) < 0) ||
                     (string_concat(&result, str_value) != 0))
            {
                LogError("Failure building amqp value string");
                free(result);
                result = NULL;
            }
            break;
        }
        case AMQP_TYPE_SHORT:
        {
            char str_value[7];
            int16_t value;
            if (amqpvalue_get_short(amqp_value, &value) != 0)
            {
                LogError("Failure getting short value");
                free(result);
                result = NULL;
            }
            else if ((sprintf(str_value, "%" PRId16, value) < 0) ||
                     (string_concat(&result, str_value) != 0))
            {
                LogError("Failure building amqp value string");
                free(result);
                result = NULL;
            }
            break;
        }
        case AMQP_TYPE_INT:
        {
            char str_value[12];
            int32_t value;
            if (amqpvalue_get_int(amqp_value, &value) != 0)
            {
                LogError("Failure getting int value");
                free(result);
                result = NULL;
            }
            else if ((sprintf(str_value, "%" PRId32, value) < 0) ||
                     (string_concat(&result, str_value) != 0))
            {
                LogError("Failure building amqp value string");
                free(result);
                result = NULL;
            }
            break;
        }
        case AMQP_TYPE_LONG:
        {
            char str_value[21];
            int64_t value;
            if (amqpvalue_get_long(amqp_value, &value) != 0)
            {
                LogError("Failure getting long value");
                free(result);
                result = NULL;
            }
            else if ((sprintf(str_value, "%" PRId64, value) < 0) ||
                     (string_concat(&result, str_value) != 0))
            {
                LogError("Failure building amqp value string");
                free(result);
                result = NULL;
            }
            break;
        }
        case AMQP_TYPE_FLOAT:
        {
            float float_value;
            if (amqpvalue_get_float(amqp_value, &float_value) != 0)
            {
                LogError("Failure getting float value");
                free(result);
                result = NULL;
            }
            else
            {
                char str_value[25];
                if ((snprintf(str_value, sizeof(str_value), "%.02f", float_value) < 0) ||
                    (string_concat(&result, str_value) != 0))
                {
                    LogError("Failure building amqp value string");
                    free(result);
                    result = NULL;
                }
            }
            break;
        }
        case AMQP_TYPE_DOUBLE:
        {
            double double_value;
            if (amqpvalue_get_double(amqp_value, &double_value) != 0)
            {
                LogError("Failure getting double value");
                free(result);
                result = NULL;
            }
            else
            {
                char str_value[25];
                if ((snprintf(str_value, sizeof(str_value), "%.02lf", double_value) < 0) ||
                    (string_concat(&result, str_value) != 0))
                {
                    LogError("Failure building amqp value string");
                    free(result);
                    result = NULL;
                }
            }
            break;
        }
        case AMQP_TYPE_CHAR:
        {
            uint32_t char_code;
            if (amqpvalue_get_char(amqp_value, &char_code) != 0)
            {
                LogError("Failure getting char value");
                free(result);
                result = NULL;
            }
            else
            {
                char str_value[25];
                if ((snprintf(str_value, sizeof(str_value), "U%02X%02X%02X%02X", char_code >> 24, (char_code >> 16) & 0xFF, (char_code >> 8) & 0xFF, char_code & 0xFF) < 0) ||
                    (string_concat(&result, str_value) != 0))
                {
                    LogError("Failure building amqp value string");
                    free(result);
                    result = NULL;
                }
            }
            break;
        }
        case AMQP_TYPE_TIMESTAMP:
        {
            char str_value[21];
            int64_t value;
            if (amqpvalue_get_timestamp(amqp_value, &value) != 0)
            {
                LogError("Failure getting timestamp value");
                free(result);
                result = NULL;
            }
            else if ((sprintf(str_value, "%" PRId64, value) < 0) ||
                     (string_concat(&result, str_value) != 0))
            {
                LogError("Failure building amqp value string");
                free(result);
                result = NULL;
            }
            break;
        }
        case AMQP_TYPE_UUID:
        {
            uuid uuid_value;
            if (amqpvalue_get_uuid(amqp_value, &uuid_value) != 0)
            {
                LogError("Failure getting uuid value");
                free(result);
                result = NULL;
            }
            else
            {
                char* uuid_string_value = UUID_to_string((const UUID_T *)&uuid_value);
                if (uuid_string_value == NULL)
                {
                    LogError("Failure getting UUID stringified value");
                    free(result);
                    result = NULL;
                }
                else
                {
                    if (string_concat(&result, uuid_string_value) != 0)
                    {
                        LogError("Failure building amqp value string");
                        free(result);
                        result = NULL;
                    }

                    free(uuid_string_value);
                }
            }

            break;
        }

        case AMQP_TYPE_BINARY:
        {
            amqp_binary binary_value;
            if (amqpvalue_get_binary(amqp_value, &binary_value) != 0)
            {
                LogError("Failure getting binary value");
                free(result);
                result = NULL;
            }
            else
            {
                if (string_concat(&result, "<") != 0)
                {
                    LogError("Failure building amqp value string");
                    free(result);
                    result = NULL;
                }
                else
                {
                    uint64_t i;

                    for (i = 0; i < binary_value.length; i++)
                    {
                        char str_value[4];
                        if ((snprintf(str_value, sizeof(str_value), "%s%02X", (i > 0) ? " " : "", ((unsigned char*)binary_value.bytes)[i]) < 0) ||
                            (string_concat(&result, str_value) != 0))
                        {
                            break;
                        }
                    }

                    if ((i < binary_value.length) ||
                        (string_concat(&result, ">") != 0))
                    {
                        LogError("Failure building amqp value string");
                        free(result);
                        result = NULL;
                    }
                }
            }
            break;
        }
        case AMQP_TYPE_STRING:
        {
            const char* string_value;
            if (amqpvalue_get_string(amqp_value, &string_value) != 0)
            {
                LogError("Failure getting string value");
                free(result);
                result = NULL;
            }
            else
            {
                if (string_concat(&result, string_value) != 0)
                {
                    LogError("Failure building amqp value string");
                    free(result);
                    result = NULL;
                }
            }
            break;
        }
        case AMQP_TYPE_SYMBOL:
        {
            const char* string_value;
            if (amqpvalue_get_symbol(amqp_value, &string_value) != 0)
            {
                LogError("Failure getting symbol value");
                free(result);
                result = NULL;
            }
            else
            {
                if (string_concat(&result, string_value) != 0)
                {
                    LogError("Failure building amqp value string");
                    free(result);
                    result = NULL;
                }
            }
            break;
        }
        case AMQP_TYPE_LIST:
        {
            uint32_t count;
            if (amqpvalue_get_list_item_count(amqp_value, &count) != 0)
            {
                LogError("Failure getting list item count value");
                free(result);
                result = NULL;
            }
            else if (string_concat(&result, "{") != 0)
            {
                LogError("Failure building amqp value string");
                free(result);
                result = NULL;
            }
            else
            {
                size_t i;
                for (i = 0; i < count; i++)
                {
                    AMQP_VALUE item = amqpvalue_get_list_item(amqp_value, i);
                    if (item == NULL)
                    {
                        LogError("Failure getting item %u from list", (unsigned int)i);
                        break;
                    }
                    else
                    {
                        char* item_string = amqpvalue_to_string(item);
                        if (item_string == NULL)
                        {
                            LogError("Failure converting item %u to string", (unsigned int)i);
                            amqpvalue_destroy(item);
                            break;
                        }
                        else
                        {
                            if (((i > 0) && (string_concat(&result, ",") != 0)) ||
                                 (string_concat(&result, item_string) != 0))
                            {
                                LogError("Failure building amqp value string");
                                free(result);
                                result = NULL;
                                break;
                            }

                            free(item_string);
                        }

                        amqpvalue_destroy(item);
                    }
                }

                if (i < count)
                {
                    // no log here, we already logged the error
                    free(result);
                    result = NULL;
                }
                else if (string_concat(&result, "}") != 0)
                {
                    LogError("Failure building amqp value string");
                    free(result);
                    result = NULL;
                }
            }
            break;
        }
        case AMQP_TYPE_MAP:
        {
            uint32_t count;
            if (amqpvalue_get_map_pair_count(amqp_value, &count) != 0)
            {
                LogError("Failure getting map pair count");
                free(result);
                result = NULL;
            }
            else if (string_concat(&result, "{") != 0)
            {
                LogError("Failure building amqp value string");
                free(result);
                result = NULL;
            }
            else
            {
                uint32_t i;
                for (i = 0; i < count; i++)
                {
                    AMQP_VALUE key;
                    AMQP_VALUE value;
                    if (amqpvalue_get_map_key_value_pair(amqp_value, i, &key, &value) != 0)
                    {
                        LogError("Failure getting key/value pair index %u", (unsigned int)i);
                        break;
                    }
                    else
                    {
                        char* key_string = amqpvalue_to_string(key);
                        if (key_string == NULL)
                        {
                            LogError("Failure getting stringified key value for index %u", (unsigned int)i);
                            amqpvalue_destroy(key);
                            amqpvalue_destroy(value);
                            break;
                        }
                        else
                        {
                            char* value_string = amqpvalue_to_string(value);
                            if (value_string == NULL)
                            {
                                LogError("Failure getting stringified value for index %u", (unsigned int)i);
                                free(key_string);
                                amqpvalue_destroy(key);
                                amqpvalue_destroy(value);
                                break;
                            }
                            else
                            {
                                if (((i > 0) && (string_concat(&result, ",") != 0)) ||
                                    (string_concat(&result, "[") != 0) ||
                                    (string_concat(&result, key_string) != 0) ||
                                    (string_concat(&result, ":") != 0) ||
                                    (string_concat(&result, value_string) != 0) ||
                                    (string_concat(&result, "]") != 0))
                                {
                                    LogError("Failure building amqp value string");
                                    free(key_string);
                                    free(value_string);
                                    amqpvalue_destroy(key);
                                    amqpvalue_destroy(value);
                                    break;
                                }

                                free(value_string);
                            }

                            free(key_string);
                        }

                        amqpvalue_destroy(key);
                        amqpvalue_destroy(value);
                    }
                }

                if (i < count)
                {
                    // no log here, we already logged the error
                    free(result);
                    result = NULL;
                }
                else if (string_concat(&result, "}") != 0)
                {
                    LogError("Failure building amqp value string");
                    free(result);
                    result = NULL;
                }
            }
            break;
        }
        case AMQP_TYPE_ARRAY:
        {
            uint32_t count;
            if (amqpvalue_get_array_item_count(amqp_value, &count) != 0)
            {
                LogError("Failure getting array item count");
                free(result);
                result = NULL;
            }
            else if (string_concat(&result, "{") != 0)
            {
                LogError("Failure building amqp value string");
                free(result);
                result = NULL;
            }
            else
            {
                uint32_t i;
                for (i = 0; i < count; i++)
                {
                    AMQP_VALUE item = amqpvalue_get_array_item(amqp_value, i);
                    if (item == NULL)
                    {
                        LogError("Failure getting array item for index %u", (unsigned int)i);
                        break;
                    }
                    else
                    {
                        char* item_string = amqpvalue_to_string(item);
                        if (item_string == NULL)
                        {
                            LogError("Failure getting stringified array item value for index %u", (unsigned int)i);
                            amqpvalue_destroy(item);
                            break;
                        }
                        else
                        {
                            if (((i > 0) && (string_concat(&result, ",") != 0)) ||
                                 (string_concat(&result, item_string) != 0))
                            {
                                LogError("Failure building amqp value string");
                                free(result);
                                result = NULL;
                                break;
                            }

                            free(item_string);
                        }

                        amqpvalue_destroy(item);
                    }
                }

                if (i < count)
                {
                    // no log here, we already logged the error
                    free(result);
                    result = NULL;
                }
                else if (string_concat(&result, "}") != 0)
                {
                    LogError("Failure building amqp value string");
                    free(result);
                    result = NULL;
                }
            }
            break;
        }
        case AMQP_TYPE_COMPOSITE:
        case AMQP_TYPE_DESCRIBED:
        {
            AMQP_VALUE described_value = amqpvalue_get_inplace_described_value(amqp_value);
            if (described_value == NULL)
            {
                LogError("Failure getting described value");
                free(result);
                result = NULL;
            }
            else
            {
                if (string_concat(&result, "* ") != 0)
                {
                    LogError("Failure building amqp value string");
                    free(result);
                    result = NULL;
                }
                else
                {
                    char* described_value_string = amqpvalue_to_string(described_value);
                    if (described_value_string == NULL)
                    {
                        LogError("Failure getting stringified described value");
                        free(result);
                        result = NULL;
                    }
                    else
                    {
                        if (string_concat(&result, described_value_string) != 0)
                        {
                            LogError("Failure building amqp value string");
                            free(result);
                            result = NULL;
                        }

                        free(described_value_string);
                    }
                }
            }
            break;
        }
        }
    }

    return result;
}
