// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>
#include <string.h>
#include "azure_c_shared_utility/gballoc.h"
#include "azure_c_shared_utility/urlencode.h"
#include "azure_c_shared_utility/xlogging.h"
#include "azure_c_shared_utility/strings.h"
#include "azure_c_shared_utility/crt_abstractions.h"

#define NIBBLE_TO_STRING(c) (char)((c) < 10 ? (c) + '0' : (c) - 10 + 'a')
#define NIBBLE_FROM_STRING(c) (char)(ISDIGIT(c) ? (c) - '0' : TOUPPER(c) + 10 - 'A')
#define IS_HEXDIGIT(c) (            \
    ((c >= '0') && (c <= '9')) ||   \
    ((c >= 'A') && (c <= 'F')) ||   \
    ((c >= 'a') && (c <= 'f'))      \
)
#define IS_PRINTABLE(c) (                           \
    (c == 0) ||                                     \
    (c == '!') ||                                   \
    (c == '(') || (c == ')') || (c == '*') ||       \
    (c == '-') || (c == '.') ||                     \
    ((c >= '0') && (c <= '9')) ||                   \
    ((c >= 'A') && (c <= 'Z')) ||                   \
    (c == '_') ||                                   \
    ((c >= 'a') && (c <= 'z'))                      \
)

/*The below macros are to be called on the big nibble of a hex value*/
#define IS_IN_ASCII_RANGE(c) (  \
    (c >= '0') && (c <= '7')    \
)
#define IS_IN_EXTENDED_ASCII_RANGE(c) ( \
    ((c >= '8') && (c <= '9')) ||       \
    ((c >= 'A') && (c <= 'F')) ||       \
    ((c >= 'a') && (c <= 'f'))          \
)
#define IS_IN_CONTINUATION_BYTE_RANGE(c) (  \
    (c == '8') || (c == '9') ||             \
    (c == 'A') || (c == 'B') ||             \
    (c == 'a') || (c == 'b')                \
)
#define IS_IN_LEADING_BYTE_RANGE(c) (   \
    ((c >= 'C') && (c <= 'F')) ||       \
    ((c >= 'c') && (c <= 'f'))          \
)
#define IS_IN_UNSUPPORTED_LEADING_BYTE_RANGE(c) (   \
    ((c >= 'D') && (c <= 'F')) ||                   \
    ((c >= 'd') && (c <= 'f'))                      \
)

static size_t URL_PrintableChar(unsigned char charVal, char* buffer)
{
    size_t size;
    if (IS_PRINTABLE(charVal))
    {
        buffer[0] = (char)charVal;
        size = 1;
    }
    else
    {
        char bigNibbleStr;
        char littleNibbleStr;
        unsigned char bigNibbleVal = charVal >> 4;
        unsigned char littleNibbleVal = charVal & 0x0F;

        if (bigNibbleVal >= 0x0C)
        {
            bigNibbleVal -= 0x04;
        }

        bigNibbleStr = NIBBLE_TO_STRING(bigNibbleVal);
        littleNibbleStr = NIBBLE_TO_STRING(littleNibbleVal);

        buffer[0] = '%';

        if (charVal < 0x80)
        {
            buffer[1] = bigNibbleStr;
            buffer[2] = littleNibbleStr;
            size = 3;
        }
        else
        {
            buffer[1] = 'c';
            buffer[3] = '%';
            buffer[4] = bigNibbleStr;
            buffer[5] = littleNibbleStr;
            if (charVal < 0xC0)
            {
                buffer[2] = '2';
            }
            else
            {
                buffer[2] = '3';
            }
            size = 6;
        }
    }

    return size;
}

static size_t calculateDecodedStringSize(const char* encodedString, size_t len)
{
    size_t decodedSize = 0;

    if (encodedString == NULL)
    {
        LogError("Null encoded string");
    }
    else if (len == 0)
    {
        decodedSize = 1; //for null terminator
    }
    else
    {
        size_t remaining_len = len;
        size_t next_step = 0;
        size_t i = 0;
        while (i < len)
        {
            //percent encoded character
            if (encodedString[i] == '%')
            {
                if (remaining_len < 3 || !IS_HEXDIGIT(encodedString[i+1]) || !IS_HEXDIGIT(encodedString[i+2]))
                {
                    LogError("Incomplete or invalid percent encoding");
                    break;
                }
                else if (!IS_IN_ASCII_RANGE(encodedString[i+1]))
                {
                    LogError("Out of range of characters accepted by this decoder");
                    break;
                }
                else
                {
                    decodedSize++;
                    next_step = 3;
                }
            }
            else if (!IS_PRINTABLE(encodedString[i]))
            {
                LogError("Unprintable value in encoded string");
                break;
            }
            //safe character
            else
            {
                decodedSize++;
                next_step = 1;
            }

            i += next_step;
            remaining_len -= next_step;
        }

        if (encodedString[i] != '\0') //i.e. broke out of above loop due to error
        {
            decodedSize = 0;
        }
        else
        {
            decodedSize++; //add space for the null terminator
        }
    }
    return decodedSize;
}

static unsigned char charFromNibbles(char bigNibbleStr, char littleNibbleStr)
{
    unsigned char bigNibbleVal = NIBBLE_FROM_STRING(bigNibbleStr);
    unsigned char littleNibbleVal = NIBBLE_FROM_STRING(littleNibbleStr);

    return bigNibbleVal << 4 | littleNibbleVal;
}

static void createDecodedString(const char* input, size_t input_size, char* output)
{
    /* Note that there is no danger of reckless indexing here, as calculateDecodedStringSize()
    has already checked lengths of strings to ensure the formatting is always correct*/
    size_t i = 0;
    while (i <= input_size) //the <= instead of < ensures the '\0' will be copied
    {
        if (input[i] != '%')
        {
            *output++ = input[i];
            i++;
        }
        else
        {
            *output++ = charFromNibbles(input[i+1], input[i+2]);
            i += 3;
        }
    }
}

static size_t URL_PrintableCharSize(unsigned char charVal)
{
    size_t size;
    if (IS_PRINTABLE(charVal))
    {
        size = 1;
    }
    else
    {
        if (charVal < 0x80)
        {
            size = 3;
        }
        else
        {
            size = 6;
        }
    }
    return size;
}

static STRING_HANDLE encode_url_data(const char* text)
{
    STRING_HANDLE result;
    size_t lengthOfResult = 0;
    char* encodedURL;
    unsigned char currentUnsignedChar;
    const char* iterator = text;

    /*Codes_SRS_URL_ENCODE_06_003: [If input is a zero length string then URL_Encode will return a zero length string.]*/
    do
    {
        currentUnsignedChar = (unsigned char)(*iterator++);
        lengthOfResult += URL_PrintableCharSize(currentUnsignedChar);
    } while (currentUnsignedChar != 0);

    if ((encodedURL = (char*)malloc(lengthOfResult)) == NULL)
    {
        /*Codes_SRS_URL_ENCODE_06_002: [If an error occurs during the encoding of input then URL_Encode will return NULL.]*/
        result = NULL;
        LogError("URL_Encode:: MALLOC failure on encode.");
    }
    else
    {
        size_t currentEncodePosition = 0;
        iterator = text;;
        do
        {
            currentUnsignedChar = (unsigned char)(*iterator++);
            currentEncodePosition += URL_PrintableChar(currentUnsignedChar, &encodedURL[currentEncodePosition]);
        } while (currentUnsignedChar != 0);

        result = STRING_new_with_memory(encodedURL);
        if (result == NULL)
        {
            LogError("URL_Encode:: MALLOC failure on encode.");
            free(encodedURL);
        }
    }
    return result;
}

STRING_HANDLE URL_EncodeString(const char* textEncode)
{
    STRING_HANDLE result;
    if (textEncode == NULL)
    {
        result = NULL;
    }
    else
    {
        result = encode_url_data(textEncode);
    }
    return result;
}

STRING_HANDLE URL_Encode(STRING_HANDLE input)
{
    STRING_HANDLE result;
    if (input == NULL)
    {
        /*Codes_SRS_URL_ENCODE_06_001: [If input is NULL then URL_Encode will return NULL.]*/
        result = NULL;
        LogError("URL_Encode:: NULL input");
    }
    else
    {
        result = encode_url_data(STRING_c_str(input));
    }
    return result;
}

STRING_HANDLE URL_DecodeString(const char* textDecode)
{
    STRING_HANDLE result;
    if (textDecode == NULL)
    {
        result = NULL;
    }
    else
    {
        STRING_HANDLE tempString = STRING_construct(textDecode);
        if (tempString == NULL)
        {
            result = NULL;
        }
        else
        {
            result = URL_Decode(tempString);
            STRING_delete(tempString);
        }
    }
    return result;
}

STRING_HANDLE URL_Decode(STRING_HANDLE input)
{
    STRING_HANDLE result;
    if (input == NULL)
    {
        LogError("URL_Decode:: NULL input");
        result = NULL;
    }
    else
    {
        size_t decodedStringSize;
        char* decodedString;
        const char* inputString = STRING_c_str(input);
        size_t inputLen = strlen(inputString);
        if ((decodedStringSize = calculateDecodedStringSize(inputString, inputLen)) == 0)
        {
            LogError("URL_Decode:: Invalid input string");
            result = NULL;
        }
        else if ((decodedString = (char*)malloc(decodedStringSize)) == NULL)
        {
            LogError("URL_Decode:: MALLOC failure on decode.");
            result = NULL;
        }
        else
        {
            createDecodedString(inputString, inputLen, decodedString);
            result = STRING_new_with_memory(decodedString);
            if (result == NULL)
            {
                LogError("URL_Decode:: MALLOC failure on decode");
                free(decodedString);
            }
        }
    }
    return result;
}
