 // Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifdef __cplusplus
#include <cstdlib>
#include <cstdint>
#else
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#endif

#include "azure_c_shared_utility/utf8_checker.h"

bool utf8_checker_is_valid_utf8(const unsigned char* utf8_str, size_t length)
{
    bool result;

    if (utf8_str == NULL)
    {
        /* Codes_SRS_UTF8_CHECKER_01_002: [ If utf8_checker_is_valid_utf8 is called with NULL utf8_str it shall return false. ]*/
        result = false;
    }
    else
    {
        size_t pos = 0;

        /* Codes_SRS_UTF8_CHECKER_01_003: [ If length is 0, utf8_checker_is_valid_utf8 shall consider utf8_str to be valid UTF-8 and return true. ]*/
        result = true;

        while ((result == true) &&
               (pos < length))
        {
            /* Codes_SRS_UTF8_CHECKER_01_001: [ utf8_checker_is_valid_utf8 shall verify that the sequence of chars pointed to by utf8_str represent UTF-8 encoded codepoints. ]*/
            if ((utf8_str[pos] >> 3) == 0x1E)
            {
                /* 4 bytes */
                /* Codes_SRS_UTF8_CHECKER_01_009: [ 000uuuuu zzzzyyyy yyxxxxxx 11110uuu 10uuzzzz 10yyyyyy 10xxxxxx ]*/
                uint32_t code_point = (utf8_str[pos] & 0x07);

                pos++;
                if ((pos < length) &&
                    ((utf8_str[pos] >> 6) == 0x02))
                {
                    code_point <<= 6;
                    code_point += utf8_str[pos] & 0x3F;

                    pos++;
                    if ((pos < length) &&
                        ((utf8_str[pos] >> 6) == 0x02))
                    {
                        code_point <<= 6;
                        code_point += utf8_str[pos] & 0x3F;

                        pos++;
                        if ((pos < length) &&
                            ((utf8_str[pos] >> 6) == 0x02))
                        {
                            code_point <<= 6;
                            code_point += utf8_str[pos] & 0x3F;

                            if (code_point <= 0xFFFF)
                            {
                                result = false;
                            }
                            else
                            {
                                /* Codes_SRS_UTF8_CHECKER_01_005: [ On success it shall return true. ]*/
                                result = true;
                                pos++;
                            }
                        }
                        else
                        {
                            result = false;
                        }
                    }
                    else
                    {
                        result = false;
                    }
                }
                else
                {
                    result = false;
                }
            }
            else if ((utf8_str[pos] >> 4) == 0x0E)
            {
                /* 3 bytes */
                /* Codes_SRS_UTF8_CHECKER_01_008: [ zzzzyyyy yyxxxxxx 1110zzzz 10yyyyyy 10xxxxxx ]*/
                uint32_t code_point = (utf8_str[pos] & 0x0F);

                pos++;
                if ((pos < length) &&
                    ((utf8_str[pos] >> 6) == 0x02))
                {
                    code_point <<= 6;
                    code_point += utf8_str[pos] & 0x3F;

                    pos++;
                    if ((pos < length) &&
                        ((utf8_str[pos] >> 6) == 0x02))
                    {
                        code_point <<= 6;
                        code_point += utf8_str[pos] & 0x3F;

                        if (code_point <= 0x7FF)
                        {
                            result = false;
                        }
                        else
                        {
                            /* Codes_SRS_UTF8_CHECKER_01_005: [ On success it shall return true. ]*/
                            result = true;
                            pos++;
                        }
                    }
                    else
                    {
                        result = false;
                    }
                }
                else
                {
                    result = false;
                }
            }
            else if ((utf8_str[pos] >> 5) == 0x06)
            {
                /* 2 bytes */
                /* Codes_SRS_UTF8_CHECKER_01_007: [ 00000yyy yyxxxxxx 110yyyyy 10xxxxxx ]*/
                uint32_t code_point = (utf8_str[pos] & 0x1F);

                pos++;
                if ((pos < length) &&
                    ((utf8_str[pos] >> 6) == 0x02))
                {
                    code_point <<= 6;
                    code_point += utf8_str[pos] & 0x3F;

                    if (code_point <= 0x7F)
                    {
                        result = false;
                    }
                    else
                    {
                        /* Codes_SRS_UTF8_CHECKER_01_005: [ On success it shall return true. ]*/
                        result = true;
                        pos++;
                    }
                }
                else
                {
                    result = false;
                }
            }
            else if ((utf8_str[pos] >> 7) == 0x00)
            {
                /* 1 byte */
                /* Codes_SRS_UTF8_CHECKER_01_006: [ 00000000 0xxxxxxx 0xxxxxxx ]*/
                /* Codes_SRS_UTF8_CHECKER_01_005: [ On success it shall return true. ]*/
                result = true;
                pos++;
            }
            else
            {
                /* error */
                result = false;
            }
        }
    }

    return result;
}
