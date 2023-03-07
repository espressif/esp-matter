// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>
#include "azure_c_shared_utility/gballoc.h"

#include "agenttypesystem.h"
#include <inttypes.h>

#ifdef _MSC_VER
#pragma warning(disable: 4756) /* Known warning for INFINITY */
#endif

#include <stddef.h>

#include <float.h>
#include <math.h>
#include <limits.h>
#include <errno.h>

/*if ULLONG_MAX is defined by limits.h for whatever reasons... */
#ifndef ULLONG_MAX
#define ULLONG_MAX 18446744073709551615
#endif

#include "azure_c_shared_utility/crt_abstractions.h"

#include "jsonencoder.h"
#include "multitree.h"

#include "azure_c_shared_utility/xlogging.h"

#define NaN_STRING "NaN"
#define MINUSINF_STRING "-INF"
#define PLUSINF_STRING "INF"

#ifndef _HUGE_ENUF
#define _HUGE_ENUF  1e+300    /* _HUGE_ENUF*_HUGE_ENUF must overflow */
#endif /* _HUGE_ENUF */

#ifndef INFINITY
#define INFINITY   ((float)(_HUGE_ENUF * _HUGE_ENUF))  /* causes warning C4756: overflow in constant arithmetic (by design) */
#endif /* INFINITY */

#ifndef NAN
#define NAN        ((float)(INFINITY * 0.0F))
#endif /* NAN */

#define GUID_STRING_LENGTH 38

// This is an artificial upper limit on floating point string length
// (e.g. the size of the string when printing %f). It is set to twice the
// maximum decimal precision plus 2. 1 for the decimal point and 1 for a
// sign (+/-)
// Unfortunately it is quite possible to print a float larger than this.
// An example of this would be printf("%.*f", MAX_FLOATING_POINT_STRING_LENGTH, 1.3);
// But currently no explicit requests for this exist in the file nor are
// any expected to reasonably occur when being used (numbers that hit
// this limit would be experiencing significant precision loss in storage anyway.
#define MAX_FLOATING_POINT_STRING_LENGTH (DECIMAL_DIG *2 + 2)

// This maximum length is 11 for 32 bit integers (including the sign)
// optionally increase to 21 if longs are 64 bit
#define MAX_LONG_STRING_LENGTH ( 11 + (10 * (sizeof(long)/ 8)))

// This is the maximum length for the largest 64 bit number (signed)
#define MAX_ULONG_LONG_STRING_LENGTH 20

MU_DEFINE_ENUM_STRINGS_WITHOUT_INVALID(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_RESULT_VALUES);

static int ValidateDate(int year, int month, int day);

static int NoCloneFunction(void** destination, const void* source)
{
    *destination = (void*)source;
    return 0;
}

static void NoFreeFunction(void* value)
{
    (void)value;
}


#define IS_DIGIT(a) (('0'<=(a)) &&((a)<='9'))
#define splitInt(intVal, bytePos)   (char)((intVal >> (bytePos << 3)) & 0xFF)
#define joinChars(a, b, c, d) (uint32_t)( (uint32_t)a + ((uint32_t)b << 8) + ((uint32_t)c << 16) + ((uint32_t)d << 24))

static char base64char(unsigned char val)
{
    char result;

    if (val < 26)
    {
        result = 'A' + (char)val;
    }
    else if (val < 52)
    {
        result = 'a' + ((char)val - 26);
    }
    else if (val < 62)
    {
        result = '0' + ((char)val - 52);
    }
    else if (val == 62)
    {
        result = '-';
    }
    else
    {
        result = '_';
    }

    return result;
}

static char base64b16(unsigned char val)
{
    const uint32_t base64b16values[4] = {
        joinChars('A', 'E', 'I', 'M'),
        joinChars('Q', 'U', 'Y', 'c'),
        joinChars('g', 'k', 'o', 's'),
        joinChars('w', '0', '4', '8')
    };
    return splitInt(base64b16values[val >> 2], (val & 0x03));
}

static char base64b8(unsigned char val)
{
    const uint32_t base64b8values = joinChars('A', 'Q', 'g', 'w');
    return splitInt(base64b8values, val);
};

/*creates an AGENT_DATA_TYPE containing a EDM_BOOLEAN from a int*/
AGENT_DATA_TYPES_RESULT Create_EDM_BOOLEAN_from_int(AGENT_DATA_TYPE* agentData, int v)
{
    AGENT_DATA_TYPES_RESULT result;
    /*Codes_SRS_AGENT_TYPE_SYSTEM_99_013:[All the Create_... functions shall check their parameters for validity.When an invalid parameter is detected, a code of AGENT_DATA_TYPES_INVALID_ARG shall be returned]*/
    if(agentData==NULL)
    {
        result = AGENT_DATA_TYPES_INVALID_ARG;
        LogError("(result = %s)", MU_ENUM_TO_STRING(AGENT_DATA_TYPES_RESULT, result));
    }
    else
    {
        /*Codes_SRS_AGENT_TYPE_SYSTEM_99_031:[ Creates a AGENT_DATA_TYPE representing an EDM_BOOLEAN.]*/
        agentData->type = EDM_BOOLEAN_TYPE;
        agentData->value.edmBoolean.value = (v)?(EDM_TRUE):(EDM_FALSE);
        result = AGENT_DATA_TYPES_OK;
    }
    return result;
}

/*creates an AGENT_DATA_TYPE containing a UINT8*/
AGENT_DATA_TYPES_RESULT Create_AGENT_DATA_TYPE_from_UINT8(AGENT_DATA_TYPE* agentData, uint8_t v)
{
    AGENT_DATA_TYPES_RESULT result;
    /*Codes_SRS_AGENT_TYPE_SYSTEM_99_013:[ All the Create_... functions shall check their parameters for validity. When an invalid parameter is detected, a code of AGENT_DATA_TYPES_INVALID_ARG shall be returned ].*/
    if (agentData == NULL)
    {
        result = AGENT_DATA_TYPES_INVALID_ARG;
        LogError("(result = %s)", MU_ENUM_TO_STRING(AGENT_DATA_TYPES_RESULT, result));
    }
    else
    {
        agentData->type = EDM_BYTE_TYPE;
        agentData->value.edmByte.value = v;
        result = AGENT_DATA_TYPES_OK;
    }
    return result;
}

/*Codes_SRS_AGENT_TYPE_SYSTEM_99_091:[Creates an AGENT_DATA_TYPE containing an Edm.DateTimeOffset from an EDM_DATE_TIME_OFFSET.]*/
AGENT_DATA_TYPES_RESULT Create_AGENT_DATA_TYPE_from_EDM_DATE_TIME_OFFSET(AGENT_DATA_TYPE* agentData, EDM_DATE_TIME_OFFSET v)
{
    AGENT_DATA_TYPES_RESULT result;
    /*Codes_SRS_AGENT_TYPE_SYSTEM_99_013:[ All the Create_... functions shall check their parameters for validity. When an invalid parameter is detected, a code of AGENT_DATA_TYPES_INVALID_ARG shall be returned ]*/
    if (agentData == NULL)
    {
        result = AGENT_DATA_TYPES_INVALID_ARG;
    }
    else if (ValidateDate(v.dateTime.tm_year+1900, v.dateTime.tm_mon +1 , v.dateTime.tm_mday) != 0)
    {
        /*Codes_SRS_AGENT_TYPE_SYSTEM_99_092:[ The structure shall be validated to be conforming to OData specifications (odata-abnf-construction-rules, 2013), and if found invalid, AGENT_DATA_TYPES_INVALID_ARG shall be returned.]*/
        result = AGENT_DATA_TYPES_INVALID_ARG;
    }
    else if (
        (v.dateTime.tm_hour > 23) ||
        (v.dateTime.tm_hour < 0) ||
        (v.dateTime.tm_min > 59) ||
        (v.dateTime.tm_min < 0) ||
        (v.dateTime.tm_sec > 59) ||
        (v.dateTime.tm_sec < 0)
        )
    {
        /*Codes_SRS_AGENT_TYPE_SYSTEM_99_092:[ The structure shall be validated, and if found invalid, AGENT_DATA_TYPES_INVALID_ARG shall be returned.]*/
        result = AGENT_DATA_TYPES_INVALID_ARG;
    }
    else if ((v.hasFractionalSecond) && (v.fractionalSecond > 999999999999))
    {
        /*Codes_SRS_AGENT_TYPE_SYSTEM_99_092:[ The structure shall be validated to be conforming to OData specifications (odata-abnf-construction-rules, 2013), and if found invalid, AGENT_DATA_TYPES_INVALID_ARG shall be returned.]*/
        result = AGENT_DATA_TYPES_INVALID_ARG;
    }
    else if (
        (v.hasTimeZone) &&
        (
            (v.timeZoneHour<-23) ||
            (v.timeZoneHour>23) ||
            (v.timeZoneMinute>59)
        )
    )
    {
        /*Codes_SRS_AGENT_TYPE_SYSTEM_99_092:[ The structure shall be validated to be conforming to OData specifications (odata-abnf-construction-rules, 2013), and if found invalid, AGENT_DATA_TYPES_INVALID_ARG shall be returned.]*/
        result = AGENT_DATA_TYPES_INVALID_ARG;
    }
    else
    {
        agentData->type = EDM_DATE_TIME_OFFSET_TYPE;
        agentData->value.edmDateTimeOffset = v;
        result = AGENT_DATA_TYPES_OK;
    }
    return result;
}

/*Codes_SRS_AGENT_TYPE_SYSTEM_99_094:[ Creates and AGENT_DATA_TYPE containing a EDM_GUID from an EDM_GUID]*/
AGENT_DATA_TYPES_RESULT Create_AGENT_DATA_TYPE_from_EDM_GUID(AGENT_DATA_TYPE* agentData, EDM_GUID v)
{
    AGENT_DATA_TYPES_RESULT result;
    /*Codes_SRS_AGENT_TYPE_SYSTEM_99_013:[ All the functions shall check their parameters for validity. When an invalid parameter is detected, the value AGENT_DATA_TYPES_INVALID_ARG shall be returned ].*/
    if (agentData == NULL)
    {
        result = AGENT_DATA_TYPES_INVALID_ARG;
        LogError("result = %s ", MU_ENUM_TO_STRING(AGENT_DATA_TYPES_RESULT, result));
    }
    else
    {
        /*Codes_SRS_AGENT_TYPE_SYSTEM_99_094:[ Creates and AGENT_DATA_TYPE containing a EDM_GUID from an EDM_GUID]*/
        agentData->type = EDM_GUID_TYPE;
        memmove(agentData->value.edmGuid.GUID, v.GUID, 16);
        result = AGENT_DATA_TYPES_OK;
    }
    return result;
}

/*Codes_SRS_AGENT_TYPE_SYSTEM_99_098:[ Creates an AGENT_DATA_TYPE containing a EDM_BINARY from a EDM_BINARY.]*/
AGENT_DATA_TYPES_RESULT Create_AGENT_DATA_TYPE_from_EDM_BINARY(AGENT_DATA_TYPE* agentData, EDM_BINARY v)
{
    AGENT_DATA_TYPES_RESULT result;
    /*Codes_SRS_AGENT_TYPE_SYSTEM_99_013:[ All the functions shall check their parameters for validity. When an invalid parameter is detected, the value AGENT_DATA_TYPES_INVALID_ARG shall be returned ].*/
    if (agentData == NULL)
    {
        result = AGENT_DATA_TYPES_INVALID_ARG;
        LogError("result = %s", MU_ENUM_TO_STRING(AGENT_DATA_TYPES_RESULT, result));
    }
    else
    {
        /*Codes_SRS_AGENT_TYPE_SYSTEM_99_098:[ Creates an AGENT_DATA_TYPE containing a EDM_BINARY from a EDM_BINARY.]*/
        if (v.data == NULL)
        {
            if (v.size != 0)
            {
                result = AGENT_DATA_TYPES_INVALID_ARG;
                LogError("result = %s ", MU_ENUM_TO_STRING(AGENT_DATA_TYPES_RESULT, result));
            }
            else
            {
                agentData->type = EDM_BINARY_TYPE;
                agentData->value.edmBinary.size = 0;
                agentData->value.edmBinary.data = NULL;
                result = AGENT_DATA_TYPES_OK;
            }
        }
        else
        {
            if (v.size != 0)
            {
                /*make a copy*/
                if ((agentData->value.edmBinary.data = (unsigned char*)malloc(v.size)) == NULL)
                {
                    result = AGENT_DATA_TYPES_ERROR;
                    LogError("result = %s", MU_ENUM_TO_STRING(AGENT_DATA_TYPES_RESULT, result));
                }
                else
                {
                    (void)memcpy(agentData->value.edmBinary.data, v.data, v.size);
                    agentData->type = EDM_BINARY_TYPE;
                    agentData->value.edmBinary.size = v.size;
                    result = AGENT_DATA_TYPES_OK;
                }
            }
            else
            {
                result = AGENT_DATA_TYPES_INVALID_ARG;
                LogError("result = %s", MU_ENUM_TO_STRING(AGENT_DATA_TYPES_RESULT, result));
            }
        }
    }
    return result;
}

/*scans sign, if any*/
/*if no sign, then it will set *sign to = +1*/
/*if sign, then it will set *sign to = +/-1*/
static void scanOptionalSign(const char* source, size_t sourceSize, size_t* position, int* sign)
{
    if (*position < sourceSize)
    {
        if (source[*position] == '-')
        {

            *sign = -1;
            (*position)++;
        }
        else if (source[*position] == '+')
        {
            *sign = +1;
            (*position)++;
        }
        else
        {
            *sign = +1;
        }
    }
}

/*scans a minus sign, if any*/
/*if no sign, then it will set *sign to = +1*/
/*if sign, then it will set *sign to = +/-1*/
static void scanOptionalMinusSign(const char* source, size_t sourceSize, size_t* position, int* sign)
{
    if (*position < sourceSize)
    {
        if (source[*position] == '-')
        {

            *sign = -1;
            (*position)++;
        }
        else
        {
            *sign = +1;
        }
    }
}

/*this function alawys returns 0 if it processed 1 digit*/
/*return 1 when error (such as wrong parameters)*/
static int scanMandatoryOneDigit(const char* source, size_t sourceSize, size_t* position)
{
    int result;
    if (*position < sourceSize)
    {
        if (IS_DIGIT(source[*position]))
        {
            (*position)++;
            result = 0;
        }
        else
        {
            result = 1;
        }
    }
    else
    {
        result = 1;
    }
    return result;
}

/*scans digits, if any*/
static void scanOptionalNDigits(const char* source, size_t sourceSize, size_t* position)
{
    while (*position < sourceSize)
    {
        if (IS_DIGIT(source[*position]))
        {
            (*position)++;
        }
        else
        {
            break;
        }
    }
}

/*from the string pointed to by source, having the size sourceSize, starting at initial position *position*/
/*this function will attempt to read a decimal number having an optional sign(+/-) followed by precisely N digits */
/*will return 0 if in the string there was a number and that number has been read in the *value parameter*/
/*will update position parameter to reflect the first character not belonging to the number*/
static int scanAndReadNDigitsInt(const char* source, size_t* position, int *value, size_t N)
{
    N++;
    *value = 0;
    while ((IS_DIGIT(source[*position])) &&
        (N > 0))
    {
        *value *= 10;
        *value += (source[*position] - '0');
        (*position)++;
        N--;
    }

    return N != 1;
}

/*this function alawys returns 0 if it found a dot followed by at least digit*/
/*return 1 when error (such as wrong parameters)*/
static int scanOptionalDotAndDigits(const char* source, size_t sourceSize, size_t* position)
{
    int result = 0;
    if (*position < sourceSize)
    {
        if (source[*position] == '.')
        {
            (*position)++;
            if (scanMandatoryOneDigit(source, sourceSize, position) != 0)
            {
                /* not a digit following the dot... */
                result = 1;
            }
            else
            {
                scanOptionalNDigits(source, sourceSize, position);
            }
        }
        else
        {
            /*not a dot, don't care*/
        }
    }
    return result;
}


static int scanMandatory1CapitalHexDigit(const char* source, uint8_t* value)
{
    int result = 0;
    if (('0' <= source[0]) && (source[0] <= '9'))
    {
        *value = (source[0] - '0');
    }
    else if (('A' <= source[0]) && (source[0] <= 'F'))
    {
        *value = (source[0] - 'A'+10);
    }
    else
    {
        result = 1;
    }
    return result;
}

/*this function alawys returns 0 if it found 2 hex digits, also updates the *value parameter*/
/*return 1 when error (such as wrong parameters)*/
static int scanMandatory2CapitalHexDigits(const char* source, uint8_t* value)
{
    int result;
    uint8_t temp;
    if (scanMandatory1CapitalHexDigit(source, &temp) == 0)
    {
        *value = temp*16;
        if (scanMandatory1CapitalHexDigit(source + 1, &temp) == 0)
        {
            *value += temp;
            result = 0;
        }
        else
        {
            result = 1;
        }
    }
    else
    {
        result = 2;
    }

    return result;
}

static int ValidateDecimal(const char* v, size_t vlen)
{
    int result;
    int sign = 0;
    size_t validatePosition = 0;
    scanOptionalSign(v, vlen, &validatePosition, &sign);
    if (scanMandatoryOneDigit(v, vlen, &validatePosition) != 0)
    {
        result = 1;
    }
    else
    {
        scanOptionalNDigits(v, vlen, &validatePosition);
        if (scanOptionalDotAndDigits(v, vlen, &validatePosition) != 0)
        {
            /*Codes_SRS_AGENT_TYPE_SYSTEM_99_067:[ If the string indicated by the parameter v does not match exactly an ODATA string representation, AGENT_DATA_TYPES_INVALID_ARG shall be returned.]*/
            result = 1;
        }
        else if (validatePosition != vlen) /*Trailing wrong characters*/
        {
            /*Codes_SRS_AGENT_TYPE_SYSTEM_99_067:[ If the string indicated by the parameter v does not match exactly an ODATA string representation, AGENT_DATA_TYPES_INVALID_ARG shall be returned.]*/
            result = 1;
        }
        else
        {
            result = 0;
        }
    }

    return result;
}

/*return 0 if everything went ok*/
/*takes 1 base64char and returns its value*/
static int base64toValue(char base64charSource, unsigned char* value)
{
    int result;
    if (('A' <= base64charSource) && (base64charSource <= 'Z'))
    {
        *value = base64charSource - 'A';
        result = 0;
    }
    else if (('a' <= base64charSource) && (base64charSource <= 'z'))
    {
        *value = ('Z' - 'A') +1+ (base64charSource - 'a');
        result = 0;
    }
    else if (('0' <= base64charSource) && (base64charSource <= '9'))
    {
        *value = ('Z' - 'A') +1+('z'-'a')+1 +(base64charSource - '0');
        result = 0;
    }
    else if ('-' == base64charSource)
    {
        *value = 62;
        result = 0;
    }
    else if ('_' == base64charSource)
    {
        *value = 63;
        result = 0;
    }
    else
    {
        result = 1;
    }
    return result;
}

/*returns 0 if everything went ok*/
/*scans 4 base64 characters and returns 3 usual bytes*/
static int scan4base64char(const char* source, size_t sourceSize, unsigned char *destination0, unsigned char* destination1, unsigned char* destination2)
{
    int result;
    if (sourceSize < 4)
    {
        result = 1;
    }
    else
    {
        unsigned char b0, b1, b2, b3;
        if (
            (base64toValue(source[0], &b0) == 0) &&
            (base64toValue(source[1], &b1) == 0) &&
            (base64toValue(source[2], &b2) == 0) &&
            (base64toValue(source[3], &b3) == 0)
            )
        {
            *destination0 = (b0 << 2) | ((b1 & 0x30) >> 4);
            *destination1 = ((b1 & 0x0F)<<4) | ((b2 & 0x3C) >>2 );
            *destination2 = ((b2 & 0x03) << 6) | (b3);
            result = 0;
        }
        else
        {
            result = 2;
        }
    }
    return result;
}

/*return 0 if the character is one of ( 'A' / 'E' / 'I' / 'M' / 'Q' / 'U' / 'Y' / 'c' / 'g' / 'k' / 'o' / 's' / 'w' / '0' / '4' / '8' )*/
static int base64b16toValue(unsigned char source, unsigned char* destination)
{
    unsigned char i;
    for (i = 0; i <= 15; i++)
    {
        if (base64b16(i) == source)
        {
            *destination = i;
            return 0;
        }
    }
    return 1;
}

/*return 0 if the character is one of ( 'A' / 'Q' / 'g' / 'w' )*/
static int base64b8toValue(unsigned char source, unsigned char* destination)
{
    unsigned char i;
    for (i = 0; i <= 3; i++)
    {
        if (base64b8(i) == source)
        {
            *destination = i;
            return 0;
        }
    }
    return 1;
}


/*returns 0 if everything went ok*/
/*scans 2 base64 characters + 1 special + 1 optional = and returns 2 usual bytes*/
int scanbase64b16(const char* source, size_t sourceSize, size_t *consumed, unsigned char* destination0, unsigned char* destination1)
{
    int result;
    if (sourceSize < 3)
    {
        result = 1;
    }
    else
    {
        unsigned char c0, c1, c2;
        *consumed = 0;
        if (
            (base64toValue(source[0], &c0) == 0) &&
            (base64toValue(source[1], &c1) == 0) &&
            (base64b16toValue(source[2], &c2) == 0)
            )
        {
            *consumed = 3 + ((sourceSize>=3)&&(source[3] == '=')); /*== produce 1 or 0 ( in C )*/
            *destination0 = (c0 << 2) | ((c1 & 0x30) >> 4);
            *destination1 = ((c1 &0x0F) <<4) | c2;
            result = 0;
        }
        else
        {
            result = 2;
        }
    }
    return result;
}

/*return 0 if everything is ok*/
/*Reads base64b8    = base64char ( 'A' / 'Q' / 'g' / 'w' ) [ "==" ]*/
int scanbase64b8(const char* source, size_t sourceSize, size_t *consumed, unsigned char* destination0)
{
    int result;
    if (sourceSize < 2)
    {
        result = 1;
    }
    else
    {
        unsigned char c0, c1;
        if (
            (base64toValue(source[0], &c0) == 0) &&
            (base64b8toValue(source[1], &c1) == 0)
            )
        {
            *consumed = 2 + (((sourceSize>=4) && (source[2] == '=') && (source[3] == '='))?2:0); /*== produce 1 or 0 ( in C )*/
            *destination0 = (c0 << 2) | (c1 & 3);
            result = 0;
        }
        else
        {
            result = 2;
        }
    }
    return result;
}

/*Codes_SRS_AGENT_TYPE_SYSTEM_99_039:[ Creates an AGENT_DATA_TYPE containing an EDM_DECIMAL from a null-terminated string.]*/
AGENT_DATA_TYPES_RESULT Create_EDM_DECIMAL_from_charz(AGENT_DATA_TYPE* agentData, const char* v)
{
    AGENT_DATA_TYPES_RESULT result;
    /*Codes_SRS_AGENT_TYPE_SYSTEM_99_013:[ All the Create_... functions shall check their parameters for validity. When an invalid parameter is detected, a code of AGENT_DATA_TYPES_INVALID_ARG shall be returned ].*/
    if (agentData == NULL)
    {
        result = AGENT_DATA_TYPES_INVALID_ARG;
        LogError("(result = %s)", MU_ENUM_TO_STRING(AGENT_DATA_TYPES_RESULT, result));
    }
    else if (v == NULL)
    {
        result = AGENT_DATA_TYPES_INVALID_ARG;
        LogError("(result = %s)", MU_ENUM_TO_STRING(AGENT_DATA_TYPES_RESULT, result));
    }
    else
    {
        size_t vlen = strlen(v);
        /*validate that v has the form [SIGN] 1*DIGIT ["." 1*DIGIT]*/
        if (vlen == 0)
        {
            /*Codes_SRS_AGENT_TYPE_SYSTEM_99_067:[ If the string indicated by the parameter v does not match exactly an ODATA string representation, AGENT_DATA_TYPES_INVALID_ARG shall be returned.]*/
            result = AGENT_DATA_TYPES_INVALID_ARG;
            LogError("(result = %s)", MU_ENUM_TO_STRING(AGENT_DATA_TYPES_RESULT, result));
        }
        else
        {
            if (ValidateDecimal(v, vlen) != 0)
            {
                /*Codes_SRS_AGENT_TYPE_SYSTEM_99_067:[ If the string indicated by the parameter v does not match exactly an ODATA string representation, AGENT_DATA_TYPES_INVALID_ARG shall be returned.]*/
                result = AGENT_DATA_TYPES_INVALID_ARG;
                LogError("(result = %s)", MU_ENUM_TO_STRING(AGENT_DATA_TYPES_RESULT, result));
            }
            else if ((agentData->value.edmDecimal.value = STRING_construct(v)) == NULL)
            {
                result = AGENT_DATA_TYPES_ERROR;
                LogError("(result = %s)", MU_ENUM_TO_STRING(AGENT_DATA_TYPES_RESULT, result));
            }
            else
            {
                agentData->type = EDM_DECIMAL_TYPE;
                result = AGENT_DATA_TYPES_OK;
            }
        }
    }
    return result;
}

/*create an AGENT_DATA_TYPE containing an EDM_DOUBLE from a double*/
AGENT_DATA_TYPES_RESULT Create_AGENT_DATA_TYPE_from_DOUBLE(AGENT_DATA_TYPE* agentData, double v)
{
    AGENT_DATA_TYPES_RESULT result;
    /*Codes_SRS_AGENT_TYPE_SYSTEM_99_013:[All the Create_... functions shall check their parameters for validity.When an invalid parameter is detected, a code of AGENT_DATA_TYPES_INVALID_ARG shall be returned]*/
    if(agentData==NULL)
    {
        result = AGENT_DATA_TYPES_INVALID_ARG;
        LogError("(result = %s)", MU_ENUM_TO_STRING(AGENT_DATA_TYPES_RESULT, result));
    }
    else
    {
        /*Codes_SRS_AGENT_TYPE_SYSTEM_99_041:[Creates an AGENT_DATA_TYPE containing an EDM_DOUBLE from double]*/
        /*Codes_SRS_AGENT_TYPE_SYSTEM_99_042:[Values of NaN, -INF, +INF are accepted]*/
        agentData->type = EDM_DOUBLE_TYPE;
        agentData->value.edmDouble.value = v;
        result = AGENT_DATA_TYPES_OK;
    }
    return result;
}

/*create an AGENT_DATA_TYPE from INT16_T*/
/*Codes_SRS_AGENT_TYPE_SYSTEM_99_043:[ Creates an AGENT_DATA_TYPE containing an EDM_INT16 from int16_t]*/
AGENT_DATA_TYPES_RESULT Create_AGENT_DATA_TYPE_from_SINT16(AGENT_DATA_TYPE* agentData, int16_t v)
{
    AGENT_DATA_TYPES_RESULT result;
    /*Codes_SRS_AGENT_TYPE_SYSTEM_99_013:[ All the Create_... functions shall check their parameters for validity. When an invalid parameter is detected, a code of AGENT_DATA_TYPES_INVALID_ARG shall be returned ].*/
    if (agentData == NULL)
    {
        result = AGENT_DATA_TYPES_INVALID_ARG;
        LogError("(result = %s)", MU_ENUM_TO_STRING(AGENT_DATA_TYPES_RESULT, result));
    }
    else
    {
        agentData->type = EDM_INT16_TYPE;
        agentData->value.edmInt16.value = v;
        result = AGENT_DATA_TYPES_OK;
    }
    return result;
}

/*create an AGENT_DATA_TYPE from INT32_T*/
AGENT_DATA_TYPES_RESULT Create_AGENT_DATA_TYPE_from_SINT32(AGENT_DATA_TYPE* agentData, int32_t v)
{
    AGENT_DATA_TYPES_RESULT result;
    /*Codes_SRS_AGENT_TYPE_SYSTEM_99_013:[ All the Create_... functions shall check their parameters for validity. When an invalid parameter is detected, a code of AGENT_DATA_TYPES_INVALID_ARG shall be returned ].*/
    if (agentData == NULL)
    {
        result = AGENT_DATA_TYPES_INVALID_ARG;
        LogError("(result = %s)", MU_ENUM_TO_STRING(AGENT_DATA_TYPES_RESULT, result));
    }
    else
    {
        agentData->type = EDM_INT32_TYPE;
        agentData->value.edmInt32.value = v;
        result = AGENT_DATA_TYPES_OK;
    }
    return result;
}

/*create an AGENT_DATA_TYPE from INT64_T*/
/*Codes_SRS_AGENT_TYPE_SYSTEM_99_045:[ Creates an AGENT_DATA_TYPE containing an EDM_INT64 from int64_t]*/
AGENT_DATA_TYPES_RESULT Create_AGENT_DATA_TYPE_from_SINT64(AGENT_DATA_TYPE* agentData, int64_t v)
{
    AGENT_DATA_TYPES_RESULT result;
    /*Codes_SRS_AGENT_TYPE_SYSTEM_99_013:[ All the Create_... functions shall check their parameters for validity. When an invalid parameter is detected, a code of AGENT_DATA_TYPES_INVALID_ARG shall be returned ].*/
    if (agentData == NULL)
    {
        result = AGENT_DATA_TYPES_INVALID_ARG;
        LogError("(result = %s)", MU_ENUM_TO_STRING(AGENT_DATA_TYPES_RESULT, result));

    }
    else
    {
        agentData->type = EDM_INT64_TYPE;
        agentData->value.edmInt64.value = v;
        result = AGENT_DATA_TYPES_OK;
    }
    return result;
}

/*create an AGENT_DATA_TYPE from int8_t*/
AGENT_DATA_TYPES_RESULT Create_AGENT_DATA_TYPE_from_SINT8(AGENT_DATA_TYPE* agentData, int8_t v)
{
    AGENT_DATA_TYPES_RESULT result;
    /*Codes_SRS_AGENT_TYPE_SYSTEM_99_013:[ All the Create_... functions shall check their parameters for validity. When an invalid parameter is detected, a code of AGENT_DATA_TYPES_INVALID_ARG shall be returned ].*/
    if (agentData == NULL)
    {
        result = AGENT_DATA_TYPES_INVALID_ARG;
        LogError("(result = %s)", MU_ENUM_TO_STRING(AGENT_DATA_TYPES_RESULT, result));

    }
    else
    {
        agentData->type = EDM_SBYTE_TYPE;
        agentData->value.edmSbyte.value = v;
        result = AGENT_DATA_TYPES_OK;
    }
    return result;
}

static int ValidateDate(int year, int month, int day)
{
    int result;

    if ((year <= -10000) || (year >= 10000))
    {
        result = 1;
    }
    else
    {
        /*Codes_SRS_AGENT_TYPE_SYSTEM_99_070:[ If year-month-date does not indicate a valid day (for example 31 Feb 2013), then AGENT_DATA_TYPES_INVALID_ARG shall be returned.]*/
        if (day <= 0)
        {
            result = 1;
        }
        else
        {
            /*the following data will be validated...*/
            /*leap years are those that can be divided by 4. But if the year can be divided by 100, it is not leap. But if they year can be divided by 400 it is leap*/
            if (
                (month == 1) || /*these months have always 31 days*/
                (month == 3) ||
                (month == 5) ||
                (month == 7) ||
                (month == 8) ||
                (month == 10) ||
                (month == 12)
                )
            {
                if (day <= 31)
                {
                    result = 0;
                }
                else
                {
                    /*Codes_SRS_AGENT_TYPE_SYSTEM_99_070:[If year - month - date does not indicate a valid day(for example 31 Feb 2013), then AGENT_DATA_TYPES_INVALID_ARG shall be returned.]*/
                    result = 1;
                }
            }
            else if (
                (month == 4) ||
                (month == 6) ||
                (month == 9) ||
                (month == 11)
                )
            {
                if (day <= 30)
                {
                    result = 0;
                }
                else
                {
                    /*Codes_SRS_AGENT_TYPE_SYSTEM_99_070:[If year - month - date does not indicate a valid day(for example 31 Feb 2013), then AGENT_DATA_TYPES_INVALID_ARG shall be returned.]*/
                    result = -1;
                }
            }
            else if (month == 2)/*february*/
            {
                if ((year % 400) == 0)
                {
                    /*these are leap years, suchs as 2000*/
                    if (day <= 29)
                    {
                        result = 0;
                    }
                    else
                    {
                        /*Codes_SRS_AGENT_TYPE_SYSTEM_99_070:[If year - month - date does not indicate a valid day(for example 31 Feb 2013), then AGENT_DATA_TYPES_INVALID_ARG shall be returned.]*/
                        result = 1;
                    }
                }
                else if ((year % 100) == 0)
                {
                    /*non-leap year, such as 1900*/
                    if (day <= 28)
                    {
                        result = 0;
                    }
                    else
                    {
                        /*Codes_SRS_AGENT_TYPE_SYSTEM_99_070:[If year - month - date does not indicate a valid day(for example 31 Feb 2013), then AGENT_DATA_TYPES_INVALID_ARG shall be returned.]*/
                        result = 1;
                    }
                }
                else if ((year % 4) == 0)
                {
                    /*these are leap years, such as 2104*/
                    if (day <= 29)
                    {
                        result = 0;
                    }
                    else
                    {
                        /*Codes_SRS_AGENT_TYPE_SYSTEM_99_070:[If year - month - date does not indicate a valid day(for example 31 Feb 2013), then AGENT_DATA_TYPES_INVALID_ARG shall be returned.]*/
                        result = 1;
                    }
                }
                else
                {
                    /*certainly not a leap year*/
                    if (day <= 28)
                    {
                        result = 0;
                    }
                    else
                    {
                        /*Codes_SRS_AGENT_TYPE_SYSTEM_99_070:[If year - month - date does not indicate a valid day(for example 31 Feb 2013), then AGENT_DATA_TYPES_INVALID_ARG shall be returned.]*/
                        result = 1;
                    }
                }
            }
            else
            {
                /*Codes_SRS_AGENT_TYPE_SYSTEM_99_070:[If year - month - date does not indicate a valid day(for example 31 Feb 2013), then AGENT_DATA_TYPES_INVALID_ARG shall be returned.]*/
                result = 1;
            }
        }
    }

    return result;
}

/*Codes_SRS_AGENT_TYPE_SYSTEM_99_069:[ Creates an AGENT_DATA_TYPE containing an EDM_DATE from a year, a month and a day of the month.]*/
AGENT_DATA_TYPES_RESULT Create_AGENT_DATA_TYPE_from_date(AGENT_DATA_TYPE* agentData, int16_t year, uint8_t month, uint8_t day)
{
    AGENT_DATA_TYPES_RESULT result;
    /*Codes_SRS_AGENT_TYPE_SYSTEM_99_013:[ All the Create_... functions shall check their parameters for validity. When an invalid parameter is detected, a code of AGENT_DATA_TYPES_INVALID_ARG shall be returned ].*/
    if (agentData == NULL)
    {
        result = AGENT_DATA_TYPES_INVALID_ARG;
        LogError("(result = %s)", MU_ENUM_TO_STRING(AGENT_DATA_TYPES_RESULT, result));
    }
    /*ODATA-ABNF: year  = [ "-" ] ( "0" 3DIGIT / oneToNine 3*DIGIT )*/
    else if (ValidateDate(year, month, day) != 0)
    {
        result = AGENT_DATA_TYPES_INVALID_ARG;
        LogError("(result = %s)", MU_ENUM_TO_STRING(AGENT_DATA_TYPES_RESULT, result));
    }
    else
    {
        /*Codes_SRS_AGENT_TYPE_SYSTEM_99_069:[ Creates an AGENT_DATA_TYPE containing an EDM_DATE from a year, a month and a day of the month.]*/
        agentData->type = EDM_DATE_TYPE;
        agentData->value.edmDate.year = year;
        agentData->value.edmDate.month = month;
        agentData->value.edmDate.day = day;
        result = AGENT_DATA_TYPES_OK;
    }

    return result;
}


/*create an AGENT_DATA_TYPE from SINGLE*/
AGENT_DATA_TYPES_RESULT Create_AGENT_DATA_TYPE_from_FLOAT(AGENT_DATA_TYPE* agentData, float v)
{
    AGENT_DATA_TYPES_RESULT result;
    /*Codes_SRS_AGENT_TYPE_SYSTEM_99_013:[All the Create_... functions shall check their parameters for validity.When an invalid parameter is detected, a code of AGENT_DATA_TYPES_INVALID_ARG shall be returned]*/
    if(agentData==NULL)
    {
        result = AGENT_DATA_TYPES_INVALID_ARG;
        LogError("(result = %s)", MU_ENUM_TO_STRING(AGENT_DATA_TYPES_RESULT, result));
    }
    else
    {
        agentData->type = EDM_SINGLE_TYPE;
        agentData->value.edmSingle.value = v;
        result = AGENT_DATA_TYPES_OK;
    }
    return result;
}

const char hexToASCII[16] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };

/*create an AGENT_DATA_TYPE from ANSI zero terminated string*/
AGENT_DATA_TYPES_RESULT Create_AGENT_DATA_TYPE_from_charz(AGENT_DATA_TYPE* agentData, const char* v)
{
    AGENT_DATA_TYPES_RESULT result = AGENT_DATA_TYPES_OK;
    /*Codes_SRS_AGENT_TYPE_SYSTEM_99_013:[ All the Create_... functions shall check their parameters for validity. When an invalid parameter is detected, a code of AGENT_DATA_TYPES_INVALID_ARG shall be returned ].*/
    if (agentData == NULL)
    {
        result = AGENT_DATA_TYPES_INVALID_ARG;
        LogError("(result = %s)", MU_ENUM_TO_STRING(AGENT_DATA_TYPES_RESULT, result));
    }
    else if (v == NULL)
    {
        result = AGENT_DATA_TYPES_INVALID_ARG;
        LogError("(result = %s)", MU_ENUM_TO_STRING(AGENT_DATA_TYPES_RESULT, result));
    }
    else
    {
        /*Codes_SRS_AGENT_TYPE_SYSTEM_99_049:[ Creates an AGENT_DATA_TYPE containing an EDM_STRING from an ASCII zero terminated string.]*/
        agentData->type = EDM_STRING_TYPE;
        if (mallocAndStrcpy_s(&agentData->value.edmString.chars, v) != 0)
        {
            result = AGENT_DATA_TYPES_ERROR;
            LogError("(result = %s)", MU_ENUM_TO_STRING(AGENT_DATA_TYPES_RESULT, result));
        }
        else
        {
            agentData->value.edmString.length = strlen(agentData->value.edmString.chars);
            result = AGENT_DATA_TYPES_OK;
        }
    }
    return result;
}

/*create an AGENT_DATA_TYPE from ANSI zero terminated string (without quotes) */
AGENT_DATA_TYPES_RESULT Create_AGENT_DATA_TYPE_from_charz_no_quotes(AGENT_DATA_TYPE* agentData, const char* v)
{
    AGENT_DATA_TYPES_RESULT result = AGENT_DATA_TYPES_OK;
    /*Codes_SRS_AGENT_TYPE_SYSTEM_99_013:[ All the Create_... functions shall check their parameters for validity. When an invalid parameter is detected, a code of AGENT_DATA_TYPES_INVALID_ARG shall be returned ].*/
    if (agentData == NULL)
    {
        result = AGENT_DATA_TYPES_INVALID_ARG;
        LogError("(result = %s)", MU_ENUM_TO_STRING(AGENT_DATA_TYPES_RESULT, result));
    }
    else if (v == NULL)
    {
        result = AGENT_DATA_TYPES_INVALID_ARG;
        LogError("(result = %s)", MU_ENUM_TO_STRING(AGENT_DATA_TYPES_RESULT, result));
    }
    else
    {
        /* Codes_SRS_AGENT_TYPE_SYSTEM_01_001: [Creates an AGENT_DATA_TYPE containing an EDM_STRING from an ASCII zero terminated string.] */
        agentData->type = EDM_STRING_NO_QUOTES_TYPE;
        if (mallocAndStrcpy_s(&agentData->value.edmStringNoQuotes.chars, v) != 0)
        {
            result = AGENT_DATA_TYPES_ERROR;
            LogError("(result = %s)", MU_ENUM_TO_STRING(AGENT_DATA_TYPES_RESULT, result));
        }
        else
        {
            agentData->value.edmStringNoQuotes.length = strlen(agentData->value.edmStringNoQuotes.chars);
            result = AGENT_DATA_TYPES_OK;
        }
    }
    return result;
}

void Destroy_AGENT_DATA_TYPE(AGENT_DATA_TYPE* agentData)
{
    if(agentData==NULL)
    {
        LogError("agentData was NULL.");
    }
    else
    {
        switch(agentData->type)
        {
            default:
            {
                LogError("invalid agentData");
                break;
            }
            case(EDM_NO_TYPE):
            {
                LogError("invalid agentData");
                break;
            }
            case(EDM_BINARY_TYPE):
            {
                /*destroying means maybe deallocating some memory*/
                free(agentData->value.edmBinary.data); /* If ptr is a null pointer, no action occurs*/
                agentData->value.edmBinary.data = NULL;
                break;
            }
            case(EDM_BOOLEAN_TYPE):
            {
                /*nothing to do*/
                break;
            }
            case(EDM_BYTE_TYPE):
            {
                /*nothing to do*/
                break;
            }
            case(EDM_DATE_TYPE):
            {
                /*nothing to do*/
                break;
            }
            case(EDM_DATE_TIME_OFFSET_TYPE):
            {
                /*nothing to do*/
                break;
            }
            case(EDM_DECIMAL_TYPE):
            {
                STRING_delete(agentData->value.edmDecimal.value);
                agentData->value.edmDecimal.value = NULL;
                break;
            }
            case(EDM_DURATION_TYPE):
            {
                LogError("EDM_DURATION_TYPE not implemented");
                break;
            }
            case(EDM_GUID_TYPE):
            {
                /*nothing to do*/
                break;
            }
            case(EDM_INT16_TYPE):
            {
                /*nothing to do*/
                break;
            }
            case(EDM_INT32_TYPE):
            {
                /*nothing to do*/
                break;
            }
            case(EDM_INT64_TYPE):
            {
                /*nothing to do*/
                break;
            }
            case(EDM_SBYTE_TYPE):
            {
                /*nothing to do*/
                break;
            }
            case(EDM_STREAM):
            {
                LogError("EDM_STREAM not implemented");
                break;
            }
            case(EDM_STRING_TYPE):
            {
                if (agentData->value.edmString.chars != NULL)
                {
                    free(agentData->value.edmString.chars);
                    agentData->value.edmString.chars = NULL;
                }
                break;
            }
            case(EDM_STRING_NO_QUOTES_TYPE) :
            {
                if (agentData->value.edmStringNoQuotes.chars != NULL)
                {
                    free(agentData->value.edmStringNoQuotes.chars);
                    agentData->value.edmStringNoQuotes.chars = NULL;
                }
                break;
            }
            case(EDM_TIME_OF_DAY_TYPE) :
            {
                LogError("EDM_TIME_OF_DAY_TYPE not implemented");
                break;
            }
            case(EDM_GEOGRAPHY_TYPE):
            {
                LogError("EDM_GEOGRAPHY_TYPE not implemented");
                break;
            }
            case(EDM_GEOGRAPHY_POINT_TYPE):
            {
                LogError("EDM_GEOGRAPHY_POINT_TYPE not implemented");
                break;
            }
            case(EDM_GEOGRAPHY_LINE_STRING_TYPE):
            {
                LogError("EDM_GEOGRAPHY_LINE_STRING_TYPE not implemented");
                break;
            }
            case(EDM_GEOGRAPHY_POLYGON_TYPE):
            {
                LogError("EDM_GEOGRAPHY_POLYGON_TYPE not implemented");
                break;
            }
            case(EDM_GEOGRAPHY_MULTI_POINT_TYPE):
            {
                LogError("EDM_GEOGRAPHY_MULTI_POINT_TYPE not implemented");
                break;
            }
            case(EDM_GEOGRAPHY_MULTI_LINE_STRING_TYPE):
            {
                LogError("EDM_GEOGRAPHY_MULTI_LINE_STRING_TYPE not implemented");
                break;
            }
            case(EDM_GEOGRAPHY_MULTI_POLYGON_TYPE):
            {
                LogError("EDM_GEOGRAPHY_MULTI_POLYGON_TYPE");
                break;
            }
            case(EDM_GEOGRAPHY_COLLECTION_TYPE):
            {
                LogError("EDM_GEOGRAPHY_COLLECTION_TYPE not implemented");
                break;
            }
            case(EDM_GEOMETRY_TYPE):
            {
                LogError("EDM_GEOMETRY_TYPE not implemented");
                break;
            }
            case(EDM_GEOMETRY_POINT_TYPE):
            {
                LogError("EDM_GEOMETRY_POINT_TYPE not implemented");
                break;
            }
            case(EDM_GEOMETRY_LINE_STRING_TYPE):
            {
                LogError("EDM_GEOMETRY_LINE_STRING_TYPE not implemented");
                break;
            }
            case(EDM_GEOMETRY_POLYGON_TYPE):
            {
                LogError("EDM_GEOMETRY_POLYGON_TYPE not implemented");
                break;
            }
            case(EDM_GEOMETRY_MULTI_POINT_TYPE):
            {
                LogError("EDM_GEOMETRY_MULTI_POINT_TYPE not implemented");
                break;
            }
            case(EDM_GEOMETRY_MULTI_LINE_STRING_TYPE):
            {
                LogError("EDM_GEOMETRY_MULTI_LINE_STRING_TYPE not implemented");
                break;
            }
            case(EDM_GEOMETRY_MULTI_POLYGON_TYPE):
            {
                LogError("EDM_GEOMETRY_MULTI_POLYGON_TYPE not implemented");
                break;
            }
            case(EDM_GEOMETRY_COLLECTION_TYPE):
            {
                LogError("EDM_GEOMETRY_COLLECTION_TYPE not implemented");
                break;
            }
            case(EDM_SINGLE_TYPE):
            {
                break;
            }
            case(EDM_DOUBLE_TYPE):
            {
                break;
            }
            case (EDM_COMPLEX_TYPE_TYPE):
            {
                /*Codes_SRS_AGENT_TYPE_SYSTEM_99_050:[Destroy_AGENT_DATA_TYPE shall deallocate all allocated resources used to represent the type.]*/
                size_t i;
                for (i = 0; i < agentData->value.edmComplexType.nMembers; i++)
                {
                    free((void*)(agentData->value.edmComplexType.fields[i].fieldName));
                    agentData->value.edmComplexType.fields[i].fieldName = NULL;
                    Destroy_AGENT_DATA_TYPE(agentData->value.edmComplexType.fields[i].value);
                    free(agentData->value.edmComplexType.fields[i].value);
                    agentData->value.edmComplexType.fields[i].value = NULL;
                }
                free(agentData->value.edmComplexType.fields);
                break;
            }
        }
        agentData->type = EDM_NO_TYPE; /*mark as detroyed*/
    }
}

static char hexDigitToChar(uint8_t hexDigit)
{
    if (hexDigit < 10) return '0' + hexDigit;
    else return ('A' - 10) + hexDigit;
}

AGENT_DATA_TYPES_RESULT AgentDataTypes_ToString(STRING_HANDLE destination, const AGENT_DATA_TYPE* value)
{
    AGENT_DATA_TYPES_RESULT result;

    /*Codes_SRS_AGENT_TYPE_SYSTEM_99_015:[If destination parameter is NULL, AgentDataTypes_ToString shall return AGENT_DATA_TYPES_INVALID_ARG.]*/
    if(destination == NULL)
    {
        result = AGENT_DATA_TYPES_INVALID_ARG;
        LogError("(result = %s)", MU_ENUM_TO_STRING(AGENT_DATA_TYPES_RESULT, result));
    }
    /*Codes_SRS_AGENT_TYPE_SYSTEM_99_053:[ If value is NULL or has been destroyed or otherwise doesn't contain valid data, AGENT_DATA_TYPES_INVALID_ARG shall be returned.]*/
    else if (value == NULL)
    {
        result = AGENT_DATA_TYPES_INVALID_ARG;
        LogError("(result = %s)", MU_ENUM_TO_STRING(AGENT_DATA_TYPES_RESULT, result));
    }
    else
    {
        switch(value->type)
        {
            default:
            {
                result = AGENT_DATA_TYPES_INVALID_ARG;
                LogError("(result = %s)", MU_ENUM_TO_STRING(AGENT_DATA_TYPES_RESULT, result));
                break;
            }
        case(EDM_NULL_TYPE) :
            {
                /*SRS_AGENT_TYPE_SYSTEM_99_101:[ EDM_NULL_TYPE shall return the unquoted string null.]*/
                if (STRING_concat(destination, "null") != 0)
                {
                    /*Codes_SRS_AGENT_TYPE_SYSTEM_99_016:[ When the value cannot be converted to a string AgentDataTypes_ToString shall return AGENT_DATA_TYPES_ERROR.]*/
                    result = AGENT_DATA_TYPES_ERROR;
                    LogError("(result = %s)", MU_ENUM_TO_STRING(AGENT_DATA_TYPES_RESULT, result));
                }
                else
                {
                    /*Codes_SRS_AGENT_TYPE_SYSTEM_99_014:[ All functions shall return AGENT_DATA_TYPES_OK when the processing is successful.]*/
                    result = AGENT_DATA_TYPES_OK;
                }
                break;
            }
        case(EDM_BOOLEAN_TYPE) :
            {
                /*Codes_SRS_AGENT_TYPE_SYSTEM_99_017:[EDM_BOOLEAN:as in(odata - abnf - construction - rules, 2013), booleanValue = "true" / "false"]*/
                if (value->value.edmBoolean.value == EDM_TRUE)
                {
                    /*SRS_AGENT_TYPE_SYSTEM_99_030:[If v is different than 0 then the AGENT_DATA_TYPE shall have the value "true".]*/
                    if (STRING_concat(destination, "true") != 0)
                    {
                        /*Codes_SRS_AGENT_TYPE_SYSTEM_99_016:[ When the value cannot be converted to a string AgentDataTypes_ToString shall return AGENT_DATA_TYPES_ERROR.]*/
                        result = AGENT_DATA_TYPES_ERROR;
                        LogError("(result = %s)", MU_ENUM_TO_STRING(AGENT_DATA_TYPES_RESULT, result));
                    }
                    else
                    {
                        /*Codes_SRS_AGENT_TYPE_SYSTEM_99_014:[ All functions shall return AGENT_DATA_TYPES_OK when the processing is successful.]*/
                        result = AGENT_DATA_TYPES_OK;
                    }
                }
                /*Codes_SRS_AGENT_TYPE_SYSTEM_99_017:[EDM_BOOLEAN:as in(odata - abnf - construction - rules, 2013), booleanValue = "true" / "false"]*/
                else if (value->value.edmBoolean.value == EDM_FALSE)
                {
                    /*Codes_SRS_AGENT_TYPE_SYSTEM_99_029:[ If v 0 then the AGENT_DATA_TYPE shall have the value "false" Boolean.]*/
                    if (STRING_concat(destination, "false") != 0)
                    {
                        /*Codes_SRS_AGENT_TYPE_SYSTEM_99_016:[ When the value cannot be converted to a string AgentDataTypes_ToString shall return AGENT_DATA_TYPES_ERROR.]*/
                        result = AGENT_DATA_TYPES_ERROR;
                        LogError("(result = %s)", MU_ENUM_TO_STRING(AGENT_DATA_TYPES_RESULT, result));
                    }
                    else
                    {
                        /*Codes_SRS_AGENT_TYPE_SYSTEM_99_014:[ All functions shall return AGENT_DATA_TYPES_OK when the processing is successful.]*/
                        result = AGENT_DATA_TYPES_OK;
                    }
                }
                else
                {
                    /*Codes_SRS_AGENT_TYPE_SYSTEM_99_053:[ If value contains invalid data, AgentDataTypes_ToString shall return AGENT_DATA_TYPES_INVALID_ARG.]*/
                    result = AGENT_DATA_TYPES_INVALID_ARG;
                    LogError("(result = %s)", MU_ENUM_TO_STRING(AGENT_DATA_TYPES_RESULT, result));
                }

                break;
            }
            case(EDM_BYTE_TYPE) :
            {
                char tempbuffer2[4]; /*because bytes can at most be 255 and that is 3 characters + 1 for '\0'*/
                size_t pos = 0;
                if (value->value.edmByte.value >= 100) tempbuffer2[pos++] = '0' + (value->value.edmByte.value / 100);
                if (value->value.edmByte.value >= 10) tempbuffer2[pos++] = '0' + (value->value.edmByte.value % 100) / 10;
                tempbuffer2[pos++] = '0' + (value->value.edmByte.value % 10);
                tempbuffer2[pos++] = '\0';

                if (STRING_concat(destination, tempbuffer2) != 0)
                {
                    result = AGENT_DATA_TYPES_ERROR;
                    LogError("(result = %s)", MU_ENUM_TO_STRING(AGENT_DATA_TYPES_RESULT, result));
                }
                else
                {
                    result = AGENT_DATA_TYPES_OK;
                }
                break;
            }
            case(EDM_DATE_TYPE) :
            {
                size_t pos = 0;
                char tempBuffer2[1 + 5 + 1 + 2 + 1 + 2 + 1+1];
                int16_t year = value->value.edmDate.year;
                tempBuffer2[pos++] = '\"';
                /*Codes_SRS_AGENT_TYPE_SYSTEM_99_068:[ EDM_DATE: dateValue = year "-" month "-" day.]*/
                if (year < 0)
                {
                    tempBuffer2[pos++] = '-';
                    year = -year;
                }

                tempBuffer2[pos++] = '0' + (char)(year / 1000);
                tempBuffer2[pos++] = '0' + (char)((year % 1000) / 100);
                tempBuffer2[pos++] = '0' + (char)((year % 100) / 10);
                tempBuffer2[pos++] = '0' + (char)(year % 10);
                tempBuffer2[pos++] = '-';
                tempBuffer2[pos++] = '0' + value->value.edmDate.month / 10;
                tempBuffer2[pos++] = '0' + value->value.edmDate.month % 10;
                tempBuffer2[pos++] = '-';
                tempBuffer2[pos++] = '0' + value->value.edmDate.day / 10;
                tempBuffer2[pos++] = '0' + value->value.edmDate.day % 10;
                tempBuffer2[pos++] = '\"';
                tempBuffer2[pos++] = '\0';

                if (STRING_concat(destination, tempBuffer2) != 0)
                {
                    result = AGENT_DATA_TYPES_ERROR;
                    LogError("(result = %s)", MU_ENUM_TO_STRING(AGENT_DATA_TYPES_RESULT, result));
                }
                else
                {
                    result = AGENT_DATA_TYPES_OK;
                }
                break;
            }
            case (EDM_DATE_TIME_OFFSET_TYPE):
            {
                /*Codes_SRS_AGENT_TYPE_SYSTEM_99_019:[ EDM_DATETIMEOFFSET: dateTimeOffsetValue = year "-" month "-" day "T" hour ":" minute [ ":" second [ "." fractionalSeconds ] ] ( "Z" / sign hour ":" minute )]*/
                /*from ABNF seems like these numbers HAVE to be padded with zeroes*/
                if (value->value.edmDateTimeOffset.hasTimeZone)
                {
                    if (value->value.edmDateTimeOffset.hasFractionalSecond)
                    {
                        size_t tempBufferSize = 1 + // \"
                            MAX_LONG_STRING_LENGTH + // %.4d
                            1 + // -
                            MAX_LONG_STRING_LENGTH + // %.2d
                            1 + // -
                            MAX_LONG_STRING_LENGTH + // %.2d
                            1 + // T
                            MAX_LONG_STRING_LENGTH + // %.2d
                            1 + // :
                            MAX_LONG_STRING_LENGTH + // %.2d
                            1 + // :
                            MAX_LONG_STRING_LENGTH + // %.2d
                            1 + // .
                            MAX_ULONG_LONG_STRING_LENGTH + // %.12llu
                            1 + MAX_LONG_STRING_LENGTH + // %+.2d
                            1 + // :
                            MAX_LONG_STRING_LENGTH + // %.2d
                            1 + //\"
                            1;  // " (terminating NULL);

                        char* tempBuffer = (char*)malloc(tempBufferSize);
                        if (tempBuffer == NULL)
                        {
                            result = AGENT_DATA_TYPES_ERROR;
                            LogError("(result = %s)", MU_ENUM_TO_STRING(AGENT_DATA_TYPES_RESULT, result));
                        }
                        else
                        {
                            if (sprintf_s(tempBuffer, tempBufferSize, "\"%.4d-%.2d-%.2dT%.2d:%.2d:%.2d.%.12llu%+.2d:%.2d\"", /*+ in printf forces the sign to appear*/
                                value->value.edmDateTimeOffset.dateTime.tm_year+1900,
                                value->value.edmDateTimeOffset.dateTime.tm_mon+1,
                                value->value.edmDateTimeOffset.dateTime.tm_mday,
                                value->value.edmDateTimeOffset.dateTime.tm_hour,
                                value->value.edmDateTimeOffset.dateTime.tm_min,
                                value->value.edmDateTimeOffset.dateTime.tm_sec,
                                value->value.edmDateTimeOffset.fractionalSecond,
                                value->value.edmDateTimeOffset.timeZoneHour,
                                value->value.edmDateTimeOffset.timeZoneMinute) < 0)
                            {
                                result = AGENT_DATA_TYPES_ERROR;
                                LogError("(result = %s)", MU_ENUM_TO_STRING(AGENT_DATA_TYPES_RESULT, result));
                            }
                            else if (STRING_concat(destination, tempBuffer) != 0)
                            {
                                result = AGENT_DATA_TYPES_ERROR;
                                LogError("(result = %s)", MU_ENUM_TO_STRING(AGENT_DATA_TYPES_RESULT, result));
                            }
                            else
                            {
                                result = AGENT_DATA_TYPES_OK;
                            }

                            // Clean up temp buffer if allocated
                            free(tempBuffer);
                        }
                    }
                    else
                    {
                        size_t tempBufferSize = 1 + // \"
                            MAX_LONG_STRING_LENGTH + // %.4d
                            1 + // -
                            MAX_LONG_STRING_LENGTH + // %.2d
                            1 + // -
                            MAX_LONG_STRING_LENGTH + // %.2d
                            1 + // T
                            MAX_LONG_STRING_LENGTH + // %.2d
                            1 + // :
                            MAX_LONG_STRING_LENGTH + // %.2d
                            1 + // :
                            MAX_LONG_STRING_LENGTH + // %.2d
                            1 + MAX_LONG_STRING_LENGTH + // %+.2d
                            1 + // :
                            MAX_LONG_STRING_LENGTH + // %.2d
                            1 + // \"
                            1; // " terminating NULL
                        char* tempBuffer = (char*)malloc(tempBufferSize);

                        if (tempBuffer == NULL)
                        {
                            result = AGENT_DATA_TYPES_ERROR;
                            LogError("(result = %s)", MU_ENUM_TO_STRING(AGENT_DATA_TYPES_RESULT, result));
                        }
                        else
                        {
                            if (sprintf_s(tempBuffer, tempBufferSize, "\"%.4d-%.2d-%.2dT%.2d:%.2d:%.2d%+.2d:%.2d\"", /*+ in printf forces the sign to appear*/
                                value->value.edmDateTimeOffset.dateTime.tm_year + 1900,
                                value->value.edmDateTimeOffset.dateTime.tm_mon+1,
                                value->value.edmDateTimeOffset.dateTime.tm_mday,
                                value->value.edmDateTimeOffset.dateTime.tm_hour,
                                value->value.edmDateTimeOffset.dateTime.tm_min,
                                value->value.edmDateTimeOffset.dateTime.tm_sec,
                                value->value.edmDateTimeOffset.timeZoneHour,
                                value->value.edmDateTimeOffset.timeZoneMinute) < 0)
                            {
                                result = AGENT_DATA_TYPES_ERROR;
                                LogError("(result = %s)", MU_ENUM_TO_STRING(AGENT_DATA_TYPES_RESULT, result));
                            }
                            else if (STRING_concat(destination, tempBuffer) != 0)
                            {
                                result = AGENT_DATA_TYPES_ERROR;
                                LogError("(result = %s)", MU_ENUM_TO_STRING(AGENT_DATA_TYPES_RESULT, result));
                            }
                            else
                            {
                                result = AGENT_DATA_TYPES_OK;
                            }

                            // Clean up temp buffer if allocated
                            free(tempBuffer);
                        }
                    }
                }
                else
                {
                    if (value->value.edmDateTimeOffset.hasFractionalSecond)
                    {
                        size_t tempBufferSize = 1 + //\"
                            MAX_LONG_STRING_LENGTH + // %.4d
                            1 + // -
                            MAX_LONG_STRING_LENGTH + // %.2d
                            1 + // -
                            MAX_LONG_STRING_LENGTH + // %.2d
                            1 + // T
                            MAX_LONG_STRING_LENGTH + // %.2d
                            1 + // :
                            MAX_LONG_STRING_LENGTH + // %.2d
                            1 + // :
                            MAX_LONG_STRING_LENGTH + // %.2d
                            1 + // .
                            MAX_ULONG_LONG_STRING_LENGTH + // %.12llu
                            1 + // Z
                            1 + // \"
                            1; // " (terminating NULL)
                        char* tempBuffer = (char*)malloc(tempBufferSize);

                        if (tempBuffer == NULL)
                        {
                            result = AGENT_DATA_TYPES_ERROR;
                            LogError("(result = %s)", MU_ENUM_TO_STRING(AGENT_DATA_TYPES_RESULT, result));
                        }
                        else
                        {
                            if (sprintf_s(tempBuffer, tempBufferSize, "\"%.4d-%.2d-%.2dT%.2d:%.2d:%.2d.%.12lluZ\"", /*+ in printf forces the sign to appear*/
                                value->value.edmDateTimeOffset.dateTime.tm_year + 1900,
                                value->value.edmDateTimeOffset.dateTime.tm_mon+1,
                                value->value.edmDateTimeOffset.dateTime.tm_mday,
                                value->value.edmDateTimeOffset.dateTime.tm_hour,
                                value->value.edmDateTimeOffset.dateTime.tm_min,
                                value->value.edmDateTimeOffset.dateTime.tm_sec,
                                value->value.edmDateTimeOffset.fractionalSecond) < 0)
                            {
                                result = AGENT_DATA_TYPES_ERROR;
                                LogError("(result = %s)", MU_ENUM_TO_STRING(AGENT_DATA_TYPES_RESULT, result));
                            }
                            else if (STRING_concat(destination, tempBuffer) != 0)
                            {
                                result = AGENT_DATA_TYPES_ERROR;
                                LogError("(result = %s)", MU_ENUM_TO_STRING(AGENT_DATA_TYPES_RESULT, result));
                            }
                            else
                            {
                                result = AGENT_DATA_TYPES_OK;
                            }

                            free(tempBuffer);
                        }
                    }
                    else
                    {
                        size_t tempBufferSize = 1 + // \"
                            MAX_LONG_STRING_LENGTH + // %.4d
                            1 + // -
                            MAX_LONG_STRING_LENGTH + // %.2d
                            1 + // -
                            MAX_LONG_STRING_LENGTH + // %.2d
                            1 + // T
                            MAX_LONG_STRING_LENGTH + // %.2d
                            1 + // :
                            MAX_LONG_STRING_LENGTH + // %.2d
                            1 + // :
                            MAX_LONG_STRING_LENGTH + // %.2d
                            1 + // Z
                            1 + // \"
                            1; // " (terminating null);

                        char* tempBuffer = (char*)malloc(tempBufferSize);

                        if (tempBuffer == NULL)
                        {
                            result = AGENT_DATA_TYPES_ERROR;
                            LogError("(result = %s)", MU_ENUM_TO_STRING(AGENT_DATA_TYPES_RESULT, result));
                        }
                        else
                        {
                            if (sprintf_s(tempBuffer, tempBufferSize, "\"%.4d-%.2d-%.2dT%.2d:%.2d:%.2dZ\"",
                                value->value.edmDateTimeOffset.dateTime.tm_year + 1900,
                                value->value.edmDateTimeOffset.dateTime.tm_mon+1,
                                value->value.edmDateTimeOffset.dateTime.tm_mday,
                                value->value.edmDateTimeOffset.dateTime.tm_hour,
                                value->value.edmDateTimeOffset.dateTime.tm_min,
                                value->value.edmDateTimeOffset.dateTime.tm_sec) < 0)
                            {
                                result = AGENT_DATA_TYPES_ERROR;
                                LogError("(result = %s)", MU_ENUM_TO_STRING(AGENT_DATA_TYPES_RESULT, result));
                            }
                            else if (STRING_concat(destination, tempBuffer) != 0)
                            {
                                result = AGENT_DATA_TYPES_ERROR;
                                LogError("(result = %s)", MU_ENUM_TO_STRING(AGENT_DATA_TYPES_RESULT, result));
                            }
                            else
                            {
                                result = AGENT_DATA_TYPES_OK;
                            }

                            free(tempBuffer);
                        }
                    }
                }
                break;
            }
            case(EDM_DECIMAL_TYPE) :
            {
                if (STRING_concat_with_STRING(destination, value->value.edmDecimal.value) != 0)
                {
                    result = AGENT_DATA_TYPES_ERROR;
                    LogError("(result = %s)", MU_ENUM_TO_STRING(AGENT_DATA_TYPES_RESULT, result));
                }
                else
                {
                    result = AGENT_DATA_TYPES_OK;
                }
                break;
            }
            case (EDM_INT16_TYPE) :
            {
                /*-32768 to +32767*/
                char buffertemp2[7]; /*because 5 digits and sign and '\0'*/
                uint16_t positiveValue;
                size_t pos = 0;
                uint16_t rank = (uint16_t)10000;
                bool foundFirstDigit = false;

                if (value->value.edmInt16.value < 0)
                {
                    buffertemp2[pos++] = '-';
                    positiveValue = -value->value.edmInt16.value;
                }
                else
                {
                    positiveValue = value->value.edmInt16.value;
                }

                while (rank >= 10)
                {
                    if ((foundFirstDigit == true) || (positiveValue / rank) > 0)
                    {
                        buffertemp2[pos++] = '0' + (char)(positiveValue / rank);
                        foundFirstDigit = true;
                    }
                    positiveValue %= rank;
                    rank /= 10;
                }
                buffertemp2[pos++] = '0' + (char)(positiveValue);
                buffertemp2[pos++] = '\0';

                if (STRING_concat(destination, buffertemp2) != 0)
                {
                    result = AGENT_DATA_TYPES_ERROR;
                    LogError("(result = %s)", MU_ENUM_TO_STRING(AGENT_DATA_TYPES_RESULT, result));
                }
                else
                {
                    result = AGENT_DATA_TYPES_OK;
                }
                break;
            }
            case (EDM_INT32_TYPE) :
            {
                /*-2147483648 to +2147483647*/
                char buffertemp2[12]; /*because 10 digits and sign and '\0'*/
                uint32_t positiveValue;
                size_t pos = 0;
                uint32_t rank = 1000000000UL;
                bool foundFirstDigit = false;

                if (value->value.edmInt32.value < 0)
                {
                    buffertemp2[pos++] = '-';
                    positiveValue = - value->value.edmInt32.value;
                }
                else
                {
                    positiveValue = value->value.edmInt32.value;
                }

                while (rank >= 10)
                {
                    if ((foundFirstDigit == true) || (positiveValue / rank) > 0)
                    {
                        buffertemp2[pos++] = '0' + (char)(positiveValue / rank);
                        foundFirstDigit = true;
                    }
                    positiveValue %= rank;
                    rank /= 10;
                }
                buffertemp2[pos++] = '0' + (char)(positiveValue);
                buffertemp2[pos++] = '\0';

                if (STRING_concat(destination, buffertemp2) != 0)
                {
                    result = AGENT_DATA_TYPES_ERROR;
                    LogError("(result = %s)", MU_ENUM_TO_STRING(AGENT_DATA_TYPES_RESULT, result));
                }
                else
                {
                    result = AGENT_DATA_TYPES_OK;
                }
                break;
            }
            case (EDM_INT64_TYPE):
            {
                char buffertemp2[21]; /*because 19 digits and sign and '\0'*/
                uint64_t positiveValue;
                size_t pos = 0;
                uint64_t rank = 10000000000000000000ULL;
                bool foundFirstDigit = false;

                if (value->value.edmInt64.value < 0)
                {
                    buffertemp2[pos++] = '-';
                    positiveValue = -value->value.edmInt64.value;
                }
                else
                {
                    positiveValue = value->value.edmInt64.value;
                }

                while (rank >= 10)
                {
                    if ((foundFirstDigit == true) || (positiveValue / rank) > 0)
                    {
                        buffertemp2[pos++] = '0' + (char)(positiveValue / rank);
                        foundFirstDigit = true;
                    }
                    positiveValue %= rank;
                    rank /= 10;
                }
                buffertemp2[pos++] = '0' + (char)(positiveValue);
                buffertemp2[pos++] = '\0';

                if (STRING_concat(destination, buffertemp2) != 0)
                {
                    result = AGENT_DATA_TYPES_ERROR;
                    LogError("(result = %s)", MU_ENUM_TO_STRING(AGENT_DATA_TYPES_RESULT, result));
                }
                else
                {
                    result = AGENT_DATA_TYPES_OK;
                }
                break;
            }
            case (EDM_SBYTE_TYPE) :
            {
                char tempbuffer2[5]; /* because '-' and 3 characters for 127 let's say and '\0'*/
                int absValue = value->value.edmSbyte.value;
                size_t pos=0;
                /*Codes_SRS_AGENT_TYPE_SYSTEM_99_026:[ EDM_SBYTE: sbyteValue = [ sign ] 1*3DIGIT  ; numbers in the range from -128 to 127]*/

                if (value->value.edmSbyte.value < 0)
                {
                    tempbuffer2[pos++] = '-';
                    absValue = -absValue;
                }

                if (absValue >= 100 ) tempbuffer2[pos++] = '0' + (char)(absValue / 100);
                if (absValue >=10) tempbuffer2[pos++] = '0' + (absValue % 100) / 10;
                tempbuffer2[pos++] = '0' + (absValue % 10);
                tempbuffer2[pos++] = '\0';


                if (STRING_concat(destination, tempbuffer2) != 0)
                {
                    result = AGENT_DATA_TYPES_ERROR;
                    LogError("(result = %s)", MU_ENUM_TO_STRING(AGENT_DATA_TYPES_RESULT, result));
                }
                else
                {
                    result = AGENT_DATA_TYPES_OK;
                }
                break;
            }
            case (EDM_STRING_TYPE):
            {
                size_t i;
                size_t nControlCharacters = 0; /*counts how many characters are to be expanded from 1 character to \uxxxx (6 characters)*/
                size_t nEscapeCharacters = 0;
                size_t vlen = value->value.edmString.length;
                char* v = value->value.edmString.chars;

                for (i = 0; i < vlen; i++)
                {
                    if ((unsigned char)v[i] >= 128) /*this be a UNICODE character begin*/
                    {
                        break;
                    }
                    else
                    {
                        if (v[i] <= 0x1F)
                        {
                            nControlCharacters++;
                        }
                        else if (
                            (v[i] == '"') ||
                            (v[i] == '\\') ||
                            (v[i] == '/')
                            )
                        {
                            nEscapeCharacters++;
                        }
                    }
                }

                if (i < vlen)
                {
                    result = AGENT_DATA_TYPES_INVALID_ARG; /*don't handle those who do not copy bit by bit to UTF8*/
                    LogError("(result = %s)", MU_ENUM_TO_STRING(AGENT_DATA_TYPES_RESULT, result));
                }
                else
                {
                    /*forward parse the string to scan for " and for \ that in JSON are \" respectively \\*/
                    size_t tempBufferSize = vlen + 5 * nControlCharacters + nEscapeCharacters + 3 + 1;
                    char* tempBuffer = (char*)malloc(tempBufferSize);
                    if (tempBuffer == NULL)
                    {
                        result = AGENT_DATA_TYPES_ERROR;
                        LogError("(result = %s)", MU_ENUM_TO_STRING(AGENT_DATA_TYPES_RESULT, result));
                    }
                    else
                    {
                        size_t w = 0;
                        tempBuffer[w++] = '"';
                        for (i = 0; i < vlen; i++)
                        {
                            if (v[i] <= 0x1F)
                            {
                                tempBuffer[w++] = '\\';
                                tempBuffer[w++] = 'u';
                                tempBuffer[w++] = '0';
                                tempBuffer[w++] = '0';
                                tempBuffer[w++] = hexToASCII[(v[i] & 0xF0) >> 4]; /*high nibble*/
                                tempBuffer[w++] = hexToASCII[v[i] & 0x0F]; /*lowNibble nibble*/
                            }
                            else if (v[i] == '"')
                            {
                                tempBuffer[w++] = '\\';
                                tempBuffer[w++] = '"';
                            }
                            else if (v[i] == '\\')
                            {
                                tempBuffer[w++] = '\\';
                                tempBuffer[w++] = '\\';
                            }
                            else if (v[i] == '/')
                            {
                                tempBuffer[w++] = '\\';
                                tempBuffer[w++] = '/';
                            }
                            else
                            {
                                tempBuffer[w++] = v[i];
                            }
                        }

#ifdef _MSC_VER
#pragma warning(suppress: 6386) /* The test Create_AGENT_DATA_TYPE_from_charz_With_2_Slashes_Succeeds verifies that Code Analysis is wrong here */
#endif
                        tempBuffer[w] = '"';
                        /*zero terminating it*/
                        tempBuffer[vlen + 5 * nControlCharacters + nEscapeCharacters + 3 - 1] = '\0';

                        if (STRING_concat(destination, tempBuffer) != 0)
                        {
                            result = AGENT_DATA_TYPES_ERROR;
                            LogError("(result = %s)", MU_ENUM_TO_STRING(AGENT_DATA_TYPES_RESULT, result));
                        }
                        else
                        {
                            result = AGENT_DATA_TYPES_OK;
                        }

                        free(tempBuffer);
                    }
                }

                break;
            }

            /* Codes_SRS_AGENT_TYPE_SYSTEM_01_003: [EDM_STRING_no_quotes: the string is copied as given when the AGENT_DATA_TYPE was created.] */
            case (EDM_STRING_NO_QUOTES_TYPE) :
            {
                /* this is a special case where we just want to copy/paste the contents, no encoding, no quotes */
                /* Codes_SRS_AGENT_TYPE_SYSTEM_01_002: [When serialized, this type is not enclosed with quotes.] */
                if (STRING_concat(destination, value->value.edmStringNoQuotes.chars) != 0)
                {
                    result = AGENT_DATA_TYPES_ERROR;
                    LogError("(result = %s)", MU_ENUM_TO_STRING(AGENT_DATA_TYPES_RESULT, result));
                }
                else
                {
                    result = AGENT_DATA_TYPES_OK;
                }

                break;
            }

#ifndef NO_FLOATS
            case(EDM_SINGLE_TYPE):
            {
                /*C89 standard says: When a float is promoted to double or long double, or a double is promoted to long double, its value is unchanged*/
                /*I read that as : when a float is NaN or Inf, it will stay NaN or INF in double representation*/

                /*The sprintf function returns the number of characters written in the array, not counting the terminating null character.*/

                if(ISNAN(value->value.edmSingle.value))
                {
                    if (STRING_concat(destination, NaN_STRING) != 0)
                    {
                        result = AGENT_DATA_TYPES_ERROR;
                        LogError("(result = %s)", MU_ENUM_TO_STRING(AGENT_DATA_TYPES_RESULT, result));
                    }
                    else
                    {
                        result = AGENT_DATA_TYPES_OK;
                    }
                }
                else if (ISNEGATIVEINFINITY(value->value.edmSingle.value))
                {
                    if (STRING_concat(destination, MINUSINF_STRING) != 0)
                    {
                        result = AGENT_DATA_TYPES_ERROR;
                        LogError("(result = %s)", MU_ENUM_TO_STRING(AGENT_DATA_TYPES_RESULT, result));
                    }
                    else
                    {
                        result = AGENT_DATA_TYPES_OK;
                    }
                }
                else if (ISPOSITIVEINFINITY(value->value.edmSingle.value))
                {
                    if (STRING_concat(destination, PLUSINF_STRING) != 0)
                    {
                        result = AGENT_DATA_TYPES_ERROR;
                        LogError("(result = %s)", MU_ENUM_TO_STRING(AGENT_DATA_TYPES_RESULT, result));
                    }
                    else
                    {
                        result = AGENT_DATA_TYPES_OK;
                    }
                }
                else
                {
                    size_t tempBufferSize = MAX_FLOATING_POINT_STRING_LENGTH;
                    char* tempBuffer = (char*)malloc(tempBufferSize);
                    if (tempBuffer == NULL)
                    {
                        result = AGENT_DATA_TYPES_ERROR;
                        LogError("(result = %s)", MU_ENUM_TO_STRING(AGENT_DATA_TYPES_RESULT, result));
                    }
                    else
                    {
                        if (sprintf_s(tempBuffer, tempBufferSize, "%.*f", FLT_DIG, (double)(value->value.edmSingle.value)) < 0)
                        {
                            result = AGENT_DATA_TYPES_ERROR;
                            LogError("(result = %s)", MU_ENUM_TO_STRING(AGENT_DATA_TYPES_RESULT, result));
                        }
                        else if (STRING_concat(destination, tempBuffer) != 0)
                        {
                            result = AGENT_DATA_TYPES_ERROR;
                            LogError("(result = %s)", MU_ENUM_TO_STRING(AGENT_DATA_TYPES_RESULT, result));
                        }
                        else
                        {
                            result = AGENT_DATA_TYPES_OK;
                        }

                        free(tempBuffer);
                    }
                }
                break;
            }
            case(EDM_DOUBLE_TYPE):
            {
                /*The sprintf function returns the number of characters written in the array, not counting the terminating null character.*/
                /*OData-ABNF says these can be used: nanInfinity = 'NaN' / '-INF' / 'INF'*/
                /*C90 doesn't declare a NaN or Inf in the standard, however, values might be NaN or Inf...*/
                /*C99 ... does*/
                /*C11 is same as C99*/
                /*Codes_SRS_AGENT_TYPE_SYSTEM_99_022:[ EDM_DOUBLE: doubleValue = decimalValue [ "e" [SIGN] 1*DIGIT ] / nanInfinity ; IEEE 754 binary64 floating-point number (15-17 decimal digits). The representation shall use DBL_DIG C #define*/
                if(ISNAN(value->value.edmDouble.value))
                {
                    if (STRING_concat(destination, NaN_STRING) != 0)
                    {
                        result = AGENT_DATA_TYPES_ERROR;
                        LogError("(result = %s)", MU_ENUM_TO_STRING(AGENT_DATA_TYPES_RESULT, result));
                    }
                    else
                    {
                        result = AGENT_DATA_TYPES_OK;
                    }
                }
                /*Codes_SRS_AGENT_TYPE_SYSTEM_99_022:[ EDM_DOUBLE: doubleValue = decimalValue [ "e" [SIGN] 1*DIGIT ] / nanInfinity ; IEEE 754 binary64 floating-point number (15-17 decimal digits). The representation shall use DBL_DIG C #define*/
                else if (ISNEGATIVEINFINITY(value->value.edmDouble.value))
                {
                    if (STRING_concat(destination, MINUSINF_STRING) != 0)
                    {
                        result = AGENT_DATA_TYPES_ERROR;
                        LogError("(result = %s)", MU_ENUM_TO_STRING(AGENT_DATA_TYPES_RESULT, result));
                    }
                    else
                    {
                        result = AGENT_DATA_TYPES_OK;
                    }
                }
                /*Codes_SRS_AGENT_TYPE_SYSTEM_99_022:[ EDM_DOUBLE: doubleValue = decimalValue [ "e" [SIGN] 1*DIGIT ] / nanInfinity ; IEEE 754 binary64 floating-point number (15-17 decimal digits). The representation shall use DBL_DIG C #define*/
                else if (ISPOSITIVEINFINITY(value->value.edmDouble.value))
                {
                    if (STRING_concat(destination, PLUSINF_STRING) != 0)
                    {
                        result = AGENT_DATA_TYPES_ERROR;
                        LogError("(result = %s)", MU_ENUM_TO_STRING(AGENT_DATA_TYPES_RESULT, result));
                    }
                    else
                    {
                        result = AGENT_DATA_TYPES_OK;
                    }
                }
                /*Codes_SRS_AGENT_TYPE_SYSTEM_99_022:[ EDM_DOUBLE: doubleValue = decimalValue [ "e" [SIGN] 1*DIGIT ] / nanInfinity ; IEEE 754 binary64 floating-point number (15-17 decimal digits). The representation shall use DBL_DIG C #define*/
                else
                {
                    size_t tempBufferSize = DECIMAL_DIG * 2;
                    char* tempBuffer = (char*)malloc(tempBufferSize);
                    if (tempBuffer == NULL)
                    {
                        result = AGENT_DATA_TYPES_ERROR;
                        LogError("(result = %s)", MU_ENUM_TO_STRING(AGENT_DATA_TYPES_RESULT, result));
                    }
                    else
                    {
                        if (sprintf_s(tempBuffer, tempBufferSize, "%.*f", DBL_DIG, value->value.edmDouble.value) < 0)
                        {
                            result = AGENT_DATA_TYPES_ERROR;
                            LogError("(result = %s)", MU_ENUM_TO_STRING(AGENT_DATA_TYPES_RESULT, result));
                        }
                        else if (STRING_concat(destination, tempBuffer) != 0)
                        {
                            result = AGENT_DATA_TYPES_ERROR;
                            LogError("(result = %s)", MU_ENUM_TO_STRING(AGENT_DATA_TYPES_RESULT, result));
                        }
                        else
                        {
                            result = AGENT_DATA_TYPES_OK;
                        }

                        free(tempBuffer);
                    }
                }
                break;
            }
#endif

            case(EDM_COMPLEX_TYPE_TYPE) :
            {
                /*to produce an EDM_COMPLEX_TYPE is a recursive process*/
                /*uses JSON encoder*/
                /*Codes_SRS_AGENT_TYPE_SYSTEM_99_062:[ If the AGENT_DATA_TYPE represents a "complex type", then the JSON marshaller shall produce the following JSON value:[...]*/
                MULTITREE_HANDLE treeHandle;
                result = AGENT_DATA_TYPES_OK;
                /*SRS_AGENT_TYPE_SYSTEM_99_016:[ When the value cannot be converted to a string AgentDataTypes_ToString shall return AGENT_DATA_TYPES_ERROR.]*/
                treeHandle = MultiTree_Create(NoCloneFunction, NoFreeFunction);
                if (treeHandle == NULL)
                {
                    result = AGENT_DATA_TYPES_ERROR;
                    LogError("(result = %s)", MU_ENUM_TO_STRING(AGENT_DATA_TYPES_RESULT, result));
                }
                else
                {
                    size_t i;
                    for (i = 0; i < value->value.edmComplexType.nMembers; i++)
                    {
                        if (MultiTree_AddLeaf(treeHandle, value->value.edmComplexType.fields[i].fieldName, value->value.edmComplexType.fields[i].value) != MULTITREE_OK)
                        {
                            /*SRS_AGENT_TYPE_SYSTEM_99_016:[ When the value cannot be converted to a string AgentDataTypes_ToString shall return AGENT_DATA_TYPES_ERROR.]*/
                            result = AGENT_DATA_TYPES_ERROR;
                            LogError("(result = %s)", MU_ENUM_TO_STRING(AGENT_DATA_TYPES_RESULT, result));
                            break;
                        }
                        else
                        {
                            /*all is fine*/
                        }
                    }

                    if (result == AGENT_DATA_TYPES_OK)
                    {
                            /*SRS_AGENT_TYPE_SYSTEM_99_016:[ When the value cannot be converted to a string AgentDataTypes_ToString shall return AGENT_DATA_TYPES_ERROR.]*/
                            if (JSONEncoder_EncodeTree(treeHandle, destination, (JSON_ENCODER_TOSTRING_FUNC)AgentDataTypes_ToString) != JSON_ENCODER_OK)
                            {
                                result = AGENT_DATA_TYPES_ERROR;
                                LogError("(result = %s)", MU_ENUM_TO_STRING(AGENT_DATA_TYPES_RESULT, result));
                            }
                            else
                            {
                                /*all is fine*/
                            }

                    }

                    MultiTree_Destroy(treeHandle);
                }
                break;
            }
            case EDM_GUID_TYPE:
            {
                char tempBuffer2[1 + 8 + 1 + 4 + 1 + 4 + 1 + 4 + 1 + 12 + 1+ 1];
                /*Codes_SRS_AGENT_TYPE_SYSTEM_99_093:[ EDM_GUID: 8HEXDIG "-" 4HEXDIG "-" 4HEXDIG "-" 4HEXDIG "-" 12HEXDIG]*/
                tempBuffer2[0] = '\"';
                tempBuffer2[1] = hexDigitToChar(value->value.edmGuid.GUID[0] / 16);
                tempBuffer2[2] = hexDigitToChar(value->value.edmGuid.GUID[0] % 16);
                tempBuffer2[3] = hexDigitToChar(value->value.edmGuid.GUID[1] / 16);
                tempBuffer2[4] = hexDigitToChar(value->value.edmGuid.GUID[1] % 16);
                tempBuffer2[5] = hexDigitToChar(value->value.edmGuid.GUID[2] / 16);
                tempBuffer2[6] = hexDigitToChar(value->value.edmGuid.GUID[2] % 16);
                tempBuffer2[7] = hexDigitToChar(value->value.edmGuid.GUID[3] / 16);
                tempBuffer2[8] = hexDigitToChar(value->value.edmGuid.GUID[3] % 16);

                tempBuffer2[9] = '-';
                tempBuffer2[10] = hexDigitToChar(value->value.edmGuid.GUID[4] / 16);
                tempBuffer2[11] = hexDigitToChar(value->value.edmGuid.GUID[4] % 16);
                tempBuffer2[12] = hexDigitToChar(value->value.edmGuid.GUID[5] / 16);
                tempBuffer2[13] = hexDigitToChar(value->value.edmGuid.GUID[5] % 16);

                tempBuffer2[14] = '-';
                tempBuffer2[15] = hexDigitToChar(value->value.edmGuid.GUID[6] / 16);
                tempBuffer2[16] = hexDigitToChar(value->value.edmGuid.GUID[6] % 16);
                tempBuffer2[17] = hexDigitToChar(value->value.edmGuid.GUID[7] / 16);
                tempBuffer2[18] = hexDigitToChar(value->value.edmGuid.GUID[7] % 16);

                tempBuffer2[19] = '-';
                tempBuffer2[20] = hexDigitToChar(value->value.edmGuid.GUID[8] / 16);
                tempBuffer2[21] = hexDigitToChar(value->value.edmGuid.GUID[8] % 16);
                tempBuffer2[22] = hexDigitToChar(value->value.edmGuid.GUID[9] / 16);
                tempBuffer2[23] = hexDigitToChar(value->value.edmGuid.GUID[9] % 16);

                tempBuffer2[24] = '-';
                tempBuffer2[25] = hexDigitToChar(value->value.edmGuid.GUID[10] / 16);
                tempBuffer2[26] = hexDigitToChar(value->value.edmGuid.GUID[10] % 16);
                tempBuffer2[27] = hexDigitToChar(value->value.edmGuid.GUID[11] / 16);
                tempBuffer2[28] = hexDigitToChar(value->value.edmGuid.GUID[11] % 16);
                tempBuffer2[29] = hexDigitToChar(value->value.edmGuid.GUID[12] / 16);
                tempBuffer2[30] = hexDigitToChar(value->value.edmGuid.GUID[12] % 16);
                tempBuffer2[31] = hexDigitToChar(value->value.edmGuid.GUID[13] / 16);
                tempBuffer2[32] = hexDigitToChar(value->value.edmGuid.GUID[13] % 16);
                tempBuffer2[33] = hexDigitToChar(value->value.edmGuid.GUID[14] / 16);
                tempBuffer2[34] = hexDigitToChar(value->value.edmGuid.GUID[14] % 16);
                tempBuffer2[35] = hexDigitToChar(value->value.edmGuid.GUID[15] / 16);
                tempBuffer2[36] = hexDigitToChar(value->value.edmGuid.GUID[15] % 16);

                tempBuffer2[37] = '\"';
                tempBuffer2[38] = '\0';

                if (STRING_concat(destination, tempBuffer2) != 0)
                {
                    result = AGENT_DATA_TYPES_ERROR;
                    LogError("(result = %s)", MU_ENUM_TO_STRING(AGENT_DATA_TYPES_RESULT, result));
                }
                else
                {
                    result = AGENT_DATA_TYPES_OK;
                }
                break;
            }
            case EDM_BINARY_TYPE:
            {
                size_t currentPosition = 0;
                char* temp;
                /*binary types */
                /*Codes_SRS_AGENT_TYPE_SYSTEM_99_099:[EDM_BINARY:= *(4base64char)[base64b16 / base64b8]]*/
                /*the following will happen*/
                /*1. the "data" of the binary shall be "eaten" 3 characters at a time and produce 4 base64 encoded characters for as long as there are more than 3 characters still to process*/
                /*2. the remaining characters (1 or 2) shall be encoded.*/
                /*there's a level of assumption that 'a' corresponds to 0b000000 and that '_' corresponds to 0b111111*/
                /*the encoding will use the optional [=] or [==] at the end of the encoded string, so that other less standard aware libraries can do their work*/
                /*these are the bits of the 3 normal bytes to be encoded*/
                size_t neededSize = 2; /*2 because starting and ending quotes */
                neededSize += (value->value.edmBinary.size == 0) ? (0) : ((((value->value.edmBinary.size-1) / 3) + 1) * 4);
                neededSize += 1; /*+1 because \0 at the end of the string*/
                if ((temp = (char*)malloc(neededSize))==NULL)
                {
                    result = AGENT_DATA_TYPES_ERROR;
                }
                else
                {
                    /*b0            b1(+1)          b2(+2)
                      7 6 5 4 3 2 1 0 7 6 5 4 3 2 1 0 7 6 5 4 3 2 1 0
                      |----c1---| |----c2---| |----c3---| |----c4---|
                    */

                    size_t destinationPointer = 0;
                    temp[destinationPointer++] = '"';
                    while (value->value.edmBinary.size - currentPosition >= 3)
                    {
                        char c1 = base64char(value->value.edmBinary.data[currentPosition] >> 2);
                        char c2 = base64char(
                            ((value->value.edmBinary.data[currentPosition] & 3) << 4) |
                                (value->value.edmBinary.data[currentPosition + 1] >> 4)
                        );
                        char c3 = base64char(
                            ((value->value.edmBinary.data[currentPosition + 1] & 0x0F) << 2) |
                                ((value->value.edmBinary.data[currentPosition + 2] >> 6) & 3)
                        );
                        char c4 = base64char(
                            value->value.edmBinary.data[currentPosition + 2] & 0x3F
                        );
                        currentPosition += 3;
                        temp[destinationPointer++] = c1;
                        temp[destinationPointer++] = c2;
                        temp[destinationPointer++] = c3;
                        temp[destinationPointer++] = c4;

                    }
                    if (value->value.edmBinary.size - currentPosition == 2)
                    {
                        char c1 = base64char(value->value.edmBinary.data[currentPosition] >> 2);
                        char c2 = base64char(
                            ((value->value.edmBinary.data[currentPosition] & 0x03) << 4) |
                                (value->value.edmBinary.data[currentPosition + 1] >> 4)
                        );
                        char c3 = base64b16(value->value.edmBinary.data[currentPosition + 1] & 0x0F);
                        temp[destinationPointer++] = c1;
                        temp[destinationPointer++] = c2;
                        temp[destinationPointer++] = c3;
                        temp[destinationPointer++] = '=';
                    }
                    else if (value->value.edmBinary.size - currentPosition == 1)
                    {
                        char c1 = base64char(value->value.edmBinary.data[currentPosition] >> 2);
                        char c2 = base64b8(value->value.edmBinary.data[currentPosition] & 0x03);
                        temp[destinationPointer++] = c1;
                        temp[destinationPointer++] = c2;
                        temp[destinationPointer++] = '=';
                        temp[destinationPointer++] = '=';
                    }

                    /*closing quote*/
                    temp[destinationPointer++] = '"';
                    /*null terminating the string*/
                    temp[destinationPointer] = '\0';

                    if (STRING_concat(destination, temp) != 0)
                    {
                        result = AGENT_DATA_TYPES_ERROR;
                        LogError("(result = %s)", MU_ENUM_TO_STRING(AGENT_DATA_TYPES_RESULT, result));
                    }
                    else
                    {
                        result = AGENT_DATA_TYPES_OK;
                    }
                    free(temp);
                }
                break;
            }
        }
    }
    return result;
}

/*return 0 if all names are different than NULL*/
static int isOneNameNULL(size_t nMemberNames, const char* const * memberNames)
{
    size_t i;
    int result = 0;
    for (i = 0; i < nMemberNames; i++)
    {
        if (memberNames[i] == NULL)
        {
            result = 1;
            break;
        }
    }
    return result;
}

/*return 0 if all names are different than NULL*/
static int areThereTwoSameNames(size_t nMemberNames, const char* const * memberNames)
{
    size_t i, j;
    int result = 0;
    for (i = 0; i < nMemberNames-1; i++)
    {
        for (j = i + 1; j < nMemberNames; j++)
        {
            if (strcmp(memberNames[i], memberNames[j]) == 0)
            {
                result = 1;
                goto out;
            }
        }
    }
out:
    return result;
}

static void DestroyHalfBakedComplexType(AGENT_DATA_TYPE* agentData)
{
    size_t i;
    if (agentData != NULL)
    {
        if (agentData->type == EDM_COMPLEX_TYPE_TYPE)
        {
            if (agentData->value.edmComplexType.fields != NULL)
            {
                for (i = 0; i < agentData->value.edmComplexType.nMembers; i++)
                {
                    if (agentData->value.edmComplexType.fields[i].fieldName != NULL)
                    {
                        free((void*)(agentData->value.edmComplexType.fields[i].fieldName));
                        agentData->value.edmComplexType.fields[i].fieldName = NULL;
                    }
                    if (agentData->value.edmComplexType.fields[i].value != NULL)
                    {
                        Destroy_AGENT_DATA_TYPE(agentData->value.edmComplexType.fields[i].value);
                        free(agentData->value.edmComplexType.fields[i].value);
                        agentData->value.edmComplexType.fields[i].value = NULL;
                    }
                }
                free(agentData->value.edmComplexType.fields);
                agentData->value.edmComplexType.fields = NULL;
            }
            agentData->type = EDM_NO_TYPE;
        }
    }
}

/* Creates an object of AGENT_DATA_TYPE_TYPE EDM_NULL_TYPE*/
AGENT_DATA_TYPES_RESULT Create_NULL_AGENT_DATA_TYPE(AGENT_DATA_TYPE* agentData)
{
    AGENT_DATA_TYPES_RESULT result;
    if (agentData == NULL)
    {
        result = AGENT_DATA_TYPES_INVALID_ARG;
        LogError("(result = %s)", MU_ENUM_TO_STRING(AGENT_DATA_TYPES_RESULT, result));
    }
    else
    {
        agentData->type = EDM_NULL_TYPE;
        result = AGENT_DATA_TYPES_OK;
    }
    return result;
}
/*creates a copy of the AGENT_DATA_TYPE*/
AGENT_DATA_TYPES_RESULT Create_AGENT_DATA_TYPE_from_AGENT_DATA_TYPE(AGENT_DATA_TYPE* dest, const AGENT_DATA_TYPE* src)
{
    AGENT_DATA_TYPES_RESULT result;
    size_t i;
    if ((dest == NULL) || (src == NULL))
    {
        result = AGENT_DATA_TYPES_INVALID_ARG;
        LogError("(result = %s)", MU_ENUM_TO_STRING(AGENT_DATA_TYPES_RESULT, result));
    }
    else
    {
        switch (src->type)
        {
            default:
            {
                result = AGENT_DATA_TYPES_NOT_IMPLEMENTED;
                LogError("(result = %s)", MU_ENUM_TO_STRING(AGENT_DATA_TYPES_RESULT, result));
                break;
            }
            case(EDM_NO_TYPE) :
            case(EDM_NULL_TYPE) :
            {
                dest->type = src->type;
                result = AGENT_DATA_TYPES_OK;
                break;
            }
            case(EDM_BOOLEAN_TYPE) :
            {
                dest->type = src->type;
                dest->value.edmBoolean= src->value.edmBoolean;
                result = AGENT_DATA_TYPES_OK;
                break;
            }
            case(EDM_BYTE_TYPE) :
            {
                dest->type = src->type;
                dest->value.edmByte= src->value.edmByte;
                result = AGENT_DATA_TYPES_OK;
                break;
            }
            case(EDM_DATE_TYPE) :
            {
                dest->type = src->type;
                dest->value.edmDate = src->value.edmDate;
                result = AGENT_DATA_TYPES_OK;
                break;
            }
            case(EDM_DATE_TIME_OFFSET_TYPE) :
            {
                dest->type = src->type;
                dest->value.edmDateTimeOffset = src->value.edmDateTimeOffset;
                result = AGENT_DATA_TYPES_OK;
                break;
            }
            case(EDM_DECIMAL_TYPE) :
            {
                if ((dest->value.edmDecimal.value = STRING_clone(src->value.edmDecimal.value)) == NULL)
                {
                    result = AGENT_DATA_TYPES_ERROR;
                    LogError("(result = %s)", MU_ENUM_TO_STRING(AGENT_DATA_TYPES_RESULT, result));
                }
                else
                {
                    dest->type = src->type;
                    result = AGENT_DATA_TYPES_OK;
                    /*all is fine*/
                }
                break;
            }
            case(EDM_DOUBLE_TYPE) :
            {
                dest->type = src->type;
                dest->value.edmDouble = src->value.edmDouble;
                result = AGENT_DATA_TYPES_OK;
                break;
            }
            case(EDM_DURATION_TYPE) :
            {
                result = AGENT_DATA_TYPES_NOT_IMPLEMENTED;
                LogError("(result = %s)", MU_ENUM_TO_STRING(AGENT_DATA_TYPES_RESULT, result));
                break;
            }
            case(EDM_GUID_TYPE) :
            {
                dest->type = src->type;
                dest->value.edmGuid = src->value.edmGuid;
                result = AGENT_DATA_TYPES_OK;
                break;
            }
            case EDM_BINARY_TYPE:
            {
                if (src->value.edmBinary.size == 0)
                {
                    dest->value.edmBinary.size = 0;
                    dest->value.edmBinary.data = NULL;
                    dest->type = EDM_BINARY_TYPE;
                    result = AGENT_DATA_TYPES_OK;
                }
                else
                {
                    if ((dest->value.edmBinary.data = (unsigned char*)malloc(src->value.edmBinary.size)) == NULL)
                    {
                        result = AGENT_DATA_TYPES_ERROR;
                    }
                    else
                    {
                        dest->value.edmBinary.size = src->value.edmBinary.size;
                        (void)memcpy(dest->value.edmBinary.data, src->value.edmBinary.data, src->value.edmBinary.size);
                        dest->type = EDM_BINARY_TYPE;
                        result = AGENT_DATA_TYPES_OK;
                    }
                }
                break;
            }
            case(EDM_INT16_TYPE) :
            {
                dest->type = src->type;
                dest->value.edmInt16 = src->value.edmInt16;
                result = AGENT_DATA_TYPES_OK;
                break;
            }
            case(EDM_INT32_TYPE) :
            {
                dest->type = src->type;
                dest->value.edmInt32 = src->value.edmInt32;
                result = AGENT_DATA_TYPES_OK;
                break;
            }
            case(EDM_INT64_TYPE) :
            {
                dest->type = src->type;
                dest->value.edmInt64 = src->value.edmInt64;
                result = AGENT_DATA_TYPES_OK;
                break;
            }
            case(EDM_SBYTE_TYPE) :
            {
                dest->type = src->type;
                dest->value.edmSbyte = src->value.edmSbyte;
                result = AGENT_DATA_TYPES_OK;
                break;
            }
            case(EDM_SINGLE_TYPE) :
            {
                dest->type = src->type;
                dest->value.edmSingle = src->value.edmSingle;
                result = AGENT_DATA_TYPES_OK;
                break;
            }
            case(EDM_STREAM) :
            {
                result = AGENT_DATA_TYPES_NOT_IMPLEMENTED;
                LogError("(result = %s)", MU_ENUM_TO_STRING(AGENT_DATA_TYPES_RESULT, result));
                break;
            } /*not supported, because what is stream?*/
            case(EDM_STRING_TYPE) :
            {
                dest->type = src->type;
                dest->value.edmString.length = src->value.edmString.length;
                if (mallocAndStrcpy_s((char**)&(dest->value.edmString.chars), (char*)src->value.edmString.chars) != 0)
                {
                    result = AGENT_DATA_TYPES_ERROR;
                    LogError("(result = %s)", MU_ENUM_TO_STRING(AGENT_DATA_TYPES_RESULT, result));
                }
                else
                {
                    result = AGENT_DATA_TYPES_OK;
                    /*all is fine*/
                }
                break;
            }
            case(EDM_STRING_NO_QUOTES_TYPE) :
            {
                dest->type = src->type;
                dest->value.edmStringNoQuotes.length = src->value.edmStringNoQuotes.length;
                if (mallocAndStrcpy_s((char**)&(dest->value.edmStringNoQuotes.chars), (char*)src->value.edmStringNoQuotes.chars) != 0)
                {
                    result = AGENT_DATA_TYPES_ERROR;
                    LogError("(result = %s)", MU_ENUM_TO_STRING(AGENT_DATA_TYPES_RESULT, result));
                }
                else
                {
                    result = AGENT_DATA_TYPES_OK;
                    /*all is fine*/
                }
                break;
            }
            case(EDM_TIME_OF_DAY_TYPE) :
            {
                result = AGENT_DATA_TYPES_NOT_IMPLEMENTED;
                LogError("(result = %s)", MU_ENUM_TO_STRING(AGENT_DATA_TYPES_RESULT, result));
                break;
            }
            case(EDM_GEOGRAPHY_TYPE) :
            {
                result = AGENT_DATA_TYPES_NOT_IMPLEMENTED;
                LogError("(result = %s)", MU_ENUM_TO_STRING(AGENT_DATA_TYPES_RESULT, result));
                break;
            } /*not supported because what is "abstract base type"*/
            case(EDM_GEOGRAPHY_POINT_TYPE) :
            {
                result = AGENT_DATA_TYPES_NOT_IMPLEMENTED;
                LogError("(result = %s)", MU_ENUM_TO_STRING(AGENT_DATA_TYPES_RESULT, result));
                break;
            }
            case(EDM_GEOGRAPHY_LINE_STRING_TYPE) :
            {
                result = AGENT_DATA_TYPES_NOT_IMPLEMENTED;
                LogError("(result = %s)", MU_ENUM_TO_STRING(AGENT_DATA_TYPES_RESULT, result));
                break;
            }
            case(EDM_GEOGRAPHY_POLYGON_TYPE) :
            {
                result = AGENT_DATA_TYPES_NOT_IMPLEMENTED;
                LogError("(result = %s)", MU_ENUM_TO_STRING(AGENT_DATA_TYPES_RESULT, result));
                break;
            }
            case(EDM_GEOGRAPHY_MULTI_POINT_TYPE) :
            {
                result = AGENT_DATA_TYPES_NOT_IMPLEMENTED;
                LogError("(result = %s)", MU_ENUM_TO_STRING(AGENT_DATA_TYPES_RESULT, result));
                break;
            }
            case(EDM_GEOGRAPHY_MULTI_LINE_STRING_TYPE) :
            {
                result = AGENT_DATA_TYPES_NOT_IMPLEMENTED;
                LogError("(result = %s)", MU_ENUM_TO_STRING(AGENT_DATA_TYPES_RESULT, result));
                break;
            }
            case(EDM_GEOGRAPHY_MULTI_POLYGON_TYPE) :
            {
                result = AGENT_DATA_TYPES_NOT_IMPLEMENTED;
                LogError("(result = %s)", MU_ENUM_TO_STRING(AGENT_DATA_TYPES_RESULT, result));
                break;
            }
            case(EDM_GEOGRAPHY_COLLECTION_TYPE) :
            {
                result = AGENT_DATA_TYPES_NOT_IMPLEMENTED;
                LogError("(result = %s)", MU_ENUM_TO_STRING(AGENT_DATA_TYPES_RESULT, result));
                break;
            }
            case(EDM_GEOMETRY_TYPE) :
            {
                result = AGENT_DATA_TYPES_NOT_IMPLEMENTED;
                LogError("(result = %s)", MU_ENUM_TO_STRING(AGENT_DATA_TYPES_RESULT, result));
                break;
            } /*not supported because what is "abstract base type"*/
            case(EDM_GEOMETRY_POINT_TYPE) :
            {
                result = AGENT_DATA_TYPES_NOT_IMPLEMENTED;
                LogError("(result = %s)", MU_ENUM_TO_STRING(AGENT_DATA_TYPES_RESULT, result));
                break;
            }
            case(EDM_GEOMETRY_LINE_STRING_TYPE) :
            {
                result = AGENT_DATA_TYPES_NOT_IMPLEMENTED;
                LogError("(result = %s)", MU_ENUM_TO_STRING(AGENT_DATA_TYPES_RESULT, result));
                break;
            }
            case(EDM_GEOMETRY_POLYGON_TYPE) :
            {
                result = AGENT_DATA_TYPES_NOT_IMPLEMENTED;
                LogError("(result = %s)", MU_ENUM_TO_STRING(AGENT_DATA_TYPES_RESULT, result));
                break;
            }
            case(EDM_GEOMETRY_MULTI_POINT_TYPE) :
            {
                result = AGENT_DATA_TYPES_NOT_IMPLEMENTED;
                LogError("(result = %s)", MU_ENUM_TO_STRING(AGENT_DATA_TYPES_RESULT, result));
                break;
            }
            case(EDM_GEOMETRY_MULTI_LINE_STRING_TYPE) :
            {
                result = AGENT_DATA_TYPES_NOT_IMPLEMENTED;
                LogError("(result = %s)", MU_ENUM_TO_STRING(AGENT_DATA_TYPES_RESULT, result));
                break;
            }
            case(EDM_GEOMETRY_MULTI_POLYGON_TYPE) :
            {
                result = AGENT_DATA_TYPES_NOT_IMPLEMENTED;
                LogError("(result = %s)", MU_ENUM_TO_STRING(AGENT_DATA_TYPES_RESULT, result));
                break;
            }
            case(EDM_GEOMETRY_COLLECTION_TYPE) :
            {
                result = AGENT_DATA_TYPES_NOT_IMPLEMENTED;
                LogError("(result = %s)", MU_ENUM_TO_STRING(AGENT_DATA_TYPES_RESULT, result));
                break;
            }
            case(EDM_COMPLEX_TYPE_TYPE) :
            {
                result = AGENT_DATA_TYPES_OK;
                /*copying a COMPLEX_TYPE means to copy all its member names and all its fields*/
                dest->type = src->type;
                if (src->value.edmComplexType.nMembers == 0)
                {
                    /*error*/
                    result = AGENT_DATA_TYPES_INVALID_ARG;
                    LogError("(result = %s)", MU_ENUM_TO_STRING(AGENT_DATA_TYPES_RESULT, result));
                }
                else
                {
                    dest->value.edmComplexType.nMembers = src->value.edmComplexType.nMembers;
                    dest->value.edmComplexType.fields = (COMPLEX_TYPE_FIELD_TYPE*)malloc(dest->value.edmComplexType.nMembers * sizeof(COMPLEX_TYPE_FIELD_TYPE));
                    if (dest->value.edmComplexType.fields == NULL)
                    {
                        result = AGENT_DATA_TYPES_ERROR;
                        LogError("(result = %s)", MU_ENUM_TO_STRING(AGENT_DATA_TYPES_RESULT, result));
                    }
                    else
                    {
                        for (i = 0; i < dest->value.edmComplexType.nMembers; i++)
                        {
                            dest->value.edmComplexType.fields[i].fieldName = NULL;
                            dest->value.edmComplexType.fields[i].value = NULL;
                        }

                        for (i = 0; i < dest->value.edmComplexType.nMembers; i++)
                        {
                            /*copy the name of this field*/
                            if (mallocAndStrcpy_s((char**)(&(dest->value.edmComplexType.fields[i].fieldName)), src->value.edmComplexType.fields[i].fieldName) != 0)
                            {
                                result = AGENT_DATA_TYPES_ERROR;
                                LogError("(result = %s)", MU_ENUM_TO_STRING(AGENT_DATA_TYPES_RESULT, result));
                                break;
                            }
                            else
                            {
                                /*field name copied success*/
                                /*field value copy follows*/
                                dest->value.edmComplexType.fields[i].value = (AGENT_DATA_TYPE*)calloc(1, sizeof(AGENT_DATA_TYPE));
                                if (dest->value.edmComplexType.fields[i].value == NULL)
                                {
                                    result = AGENT_DATA_TYPES_ERROR;
                                    LogError("(result = %s)", MU_ENUM_TO_STRING(AGENT_DATA_TYPES_RESULT, result));
                                    break;
                                }
                                else
                                {
                                    if (Create_AGENT_DATA_TYPE_from_AGENT_DATA_TYPE(dest->value.edmComplexType.fields[i].value, src->value.edmComplexType.fields[i].value) != AGENT_DATA_TYPES_OK)
                                    {
                                        result = AGENT_DATA_TYPES_ERROR;
                                        LogError("(result = %s)", MU_ENUM_TO_STRING(AGENT_DATA_TYPES_RESULT, result));
                                        break;
                                    }
                                    else
                                    {
                                        /*all is fine*/
                                    }
                                }
                            }
                        }

                        if (result != AGENT_DATA_TYPES_OK)
                        {
                            /*unbuild*/
                            DestroyHalfBakedComplexType(dest);
                        }
                    }
                }
                break;
            } /*ANY CHANGE (?!) here must be reflected in the tool providing the binary file (XML2BINARY) */
        }

        if (result != AGENT_DATA_TYPES_OK)
        {
            dest->type = EDM_NO_TYPE;
        }
    }

    return result;
}

AGENT_DATA_TYPES_RESULT Create_AGENT_DATA_TYPE_from_MemberPointers(AGENT_DATA_TYPE* agentData, const char* typeName, size_t nMembers, const char* const * memberNames, const AGENT_DATA_TYPE** memberPointerValues)
{
    AGENT_DATA_TYPES_RESULT result;

    /* Codes_SRS_AGENT_TYPE_SYSTEM_99_109:[ AGENT_DATA_TYPES_INVALID_ARG shall be returned if memberPointerValues parameter is NULL.] */
    if (memberPointerValues == NULL)
    {
        result = AGENT_DATA_TYPES_INVALID_ARG;
        LogError("(result: AGENT_DATA_TYPES_INVALID_ARG)");
    }
    else
    {
        AGENT_DATA_TYPE* values = (AGENT_DATA_TYPE*)calloc(1, (nMembers* sizeof(AGENT_DATA_TYPE)));
        if (values == NULL)
        {
            result = AGENT_DATA_TYPES_ERROR;
            LogError("(result = %s)", MU_ENUM_TO_STRING(AGENT_DATA_TYPES_RESULT, result));
        }
        else
        {
            size_t i;
            for (i = 0; i < nMembers; i++)
            {
                if (Create_AGENT_DATA_TYPE_from_AGENT_DATA_TYPE(values + i, memberPointerValues[i]) != AGENT_DATA_TYPES_OK)
                {
                    size_t j;
                    for (j = 0; j < i; j++)
                    {
                        Destroy_AGENT_DATA_TYPE(values + j);
                    }
                    break;
                }
            }

            if (i != nMembers)
            {
                result = AGENT_DATA_TYPES_ERROR;
                LogError("(result = %s)", MU_ENUM_TO_STRING(AGENT_DATA_TYPES_RESULT, result));
            }
            else
            {
                /* SRS_AGENT_TYPE_SYSTEM_99_111:[ AGENT_DATA_TYPES_OK shall be returned upon success.] */
                result = Create_AGENT_DATA_TYPE_from_Members(agentData, typeName, nMembers, memberNames, values);
                if (result != AGENT_DATA_TYPES_OK)
                {
                    LogError("(result = %s)", MU_ENUM_TO_STRING(AGENT_DATA_TYPES_RESULT, result));
                }
                for (i = 0; i < nMembers; i++)
                {
                    Destroy_AGENT_DATA_TYPE(&values[i]);
                }
            }
            free(values);
        }
    }
    return result;
}

AGENT_DATA_TYPES_RESULT Create_AGENT_DATA_TYPE_from_Members(AGENT_DATA_TYPE* agentData, const char* typeName, size_t nMembers, const char* const * memberNames, const AGENT_DATA_TYPE* memberValues)
{
    AGENT_DATA_TYPES_RESULT result;
    size_t i;
    /*Codes_SRS_AGENT_TYPE_SYSTEM_99_013:[ All the functions shall check their parameters for validity. When an invalid parameter is detected, the value AGENT_DATA_TYPES_INVALID_ARG shall be returned ]*/
    if (agentData == NULL)
    {
        result = AGENT_DATA_TYPES_INVALID_ARG;
        LogError("(result: AGENT_DATA_TYPES_INVALID_ARG)");
    }
    /*Codes_SRS_AGENT_TYPE_SYSTEM_99_055:[ If typeName is NULL, the function shall return AGENT_DATA_TYPES_INVALID_ARG .]*/
    else if (typeName==NULL)
    {
        result = AGENT_DATA_TYPES_INVALID_ARG;
        LogError("(result: AGENT_DATA_TYPES_INVALID_ARG)");
    }
    /*Codes_SRS_AGENT_TYPE_SYSTEM_99_056:[If nMembers is 0, the function shall return AGENT_DATA_TYPES_INVALID_ARG .]*/
    else if (nMembers == 0)
    {
        result = AGENT_DATA_TYPES_INVALID_ARG;
        LogError("(result = %s)", MU_ENUM_TO_STRING(AGENT_DATA_TYPES_RESULT, result));
    }
    /*Codes_SRS_AGENT_TYPE_SYSTEM_99_057:[ If memberNames is NULL, the function shall return AGENT_DATA_TYPES_INVALID_ARG .]*/
    else if (memberNames == NULL)
    {
        result = AGENT_DATA_TYPES_INVALID_ARG;
        LogError("(result = %s)", MU_ENUM_TO_STRING(AGENT_DATA_TYPES_RESULT, result));
    }
    /*Codes_SRS_AGENT_TYPE_SYSTEM_99_058:[ If any of the memberNames[i] is NULL, the function shall return AGENT_DATA_TYPES_INVALID_ARG .]*/
    else if (isOneNameNULL(nMembers, memberNames)!=0)
    {
        result = AGENT_DATA_TYPES_INVALID_ARG;
        LogError("(result = %s)", MU_ENUM_TO_STRING(AGENT_DATA_TYPES_RESULT, result));
    }
    /*Codes_SRS_AGENT_TYPE_SYSTEM_99_059:[ If memberValues is NULL, the function shall return AGENT_DATA_TYPES_INVALID_ARG .]*/
    else if (memberValues == NULL)
    {
        result = AGENT_DATA_TYPES_INVALID_ARG;
        LogError("(result = %s)", MU_ENUM_TO_STRING(AGENT_DATA_TYPES_RESULT, result));
    }
    /*Codes_SRS_AGENT_TYPE_SYSTEM_99_063:[ If there are two memberNames with the same name, then the function shall return  AGENT_DATA_TYPES_INVALID_ARG.]*/
    else if (areThereTwoSameNames(nMembers, memberNames) != 0)
    {
        result = AGENT_DATA_TYPES_INVALID_ARG;
        LogError("(result = %s)", MU_ENUM_TO_STRING(AGENT_DATA_TYPES_RESULT, result));
    }
    else
    {
        agentData->value.edmComplexType.nMembers = nMembers;
        agentData->value.edmComplexType.fields = (COMPLEX_TYPE_FIELD_TYPE*)malloc(nMembers *sizeof(COMPLEX_TYPE_FIELD_TYPE));
        if (agentData->value.edmComplexType.fields == NULL)
        {
            result = AGENT_DATA_TYPES_ERROR;
            LogError("(result = %s)", MU_ENUM_TO_STRING(AGENT_DATA_TYPES_RESULT, result));
        }
        else
        {
            result = AGENT_DATA_TYPES_OK; /*not liking this, solution might be to use a temp variable*/

            /*initialize the fields*/
            for (i = 0; i < nMembers; i++)
            {
                agentData->value.edmComplexType.fields[i].fieldName = NULL;
                agentData->value.edmComplexType.fields[i].value = NULL;
            }

            for (i = 0; i < nMembers; i++)
            {
                /*copy the name*/
                if (mallocAndStrcpy_s((char**)(&(agentData->value.edmComplexType.fields[i].fieldName)), memberNames[i]) != 0)
                {
                    result = AGENT_DATA_TYPES_ERROR;
                    LogError("(result = %s)", MU_ENUM_TO_STRING(AGENT_DATA_TYPES_RESULT, result));
                    break;
                }
                else
                {
                    /*field name was transferred successfully*/
                    /*copy the rest*/
                    agentData->value.edmComplexType.fields[i].value = (AGENT_DATA_TYPE*)calloc(1, sizeof(AGENT_DATA_TYPE));
                    if (agentData->value.edmComplexType.fields[i].value == NULL)
                    {
                        /*deallocate the name*/
                        free((void*)(agentData->value.edmComplexType.fields[i].fieldName));
                        agentData->value.edmComplexType.fields[i].fieldName = NULL;
                        result = AGENT_DATA_TYPES_ERROR;
                        LogError("(result = %s)", MU_ENUM_TO_STRING(AGENT_DATA_TYPES_RESULT, result));
                        break;
                    }
                    else
                    {
                        /*copy the values*/
                        if (Create_AGENT_DATA_TYPE_from_AGENT_DATA_TYPE(agentData->value.edmComplexType.fields[i].value, &(memberValues[i])) != AGENT_DATA_TYPES_OK)
                        {
                            result = AGENT_DATA_TYPES_ERROR;
                            LogError("(result = %s)", MU_ENUM_TO_STRING(AGENT_DATA_TYPES_RESULT, result));
                            break;
                        }
                        else
                        {
                            /*all is fine*/
                        }
                    }
                }

            }
        }

        if (result != AGENT_DATA_TYPES_OK)
        {
            /*dealloc, something went bad*/
            DestroyHalfBakedComplexType(agentData);
        }
        else
        {
            agentData->type = EDM_COMPLEX_TYPE_TYPE;
        }
    }
    return result;
}

#define isLeapYear(y) ((((y) % 400) == 0) || (((y)%4==0)&&(!((y)%100==0))))

static int daysInAllPreviousMonths(int month)
{
    int result = 0;
    for (int i = 0; i < month; i++)
    {
        switch (i)
        {
        case 0:
        case 2:
        case 4:
        case 6:
        case 7:
        case 9:
            result += 31;
            break;
        case 1:
            result += 28;
            break;
        default:
            result += 30;
        }
    }
    return result;
}

/*this function assumes a correctly filled in tm_year, tm_mon and tm_mday and will fill in tm_yday and tm_wday*/
static void fill_tm_yday_and_tm_wday(struct tm* source)
{
    /*to fill in tm_yday the function shall add the number of days in every month, not including the current one*/
    /*and then it will add the number of days in the current month*/
    /*1st of Jan is day "0" in a year*/
    int year = source->tm_year + 1900 + 10000;
    int nLeapYearsSinceYearMinus9999 = ((year - 1) / 4) - ((year - 1) / 100) + ((year - 1) / 400);
    source->tm_yday = (daysInAllPreviousMonths(source->tm_mon)) + (source->tm_mday - 1) + ((source->tm_mon > 1 /*1 is Feb*/) && isLeapYear(year));
    source->tm_wday = ((365 * year + nLeapYearsSinceYearMinus9999) + source->tm_yday) % 7;
    /*day "0" is 1st jan -9999 (this is how much odata can span*/
    /*the function shall count days */
}

/*the following function does the same as  sscanf(pos2, ":%02d", &sec)*/
/*this function only exists because of optimizing valgrind time, otherwise sscanf would be just as good*/
static int sscanf2d(const char *pos2, int* sec)
{
    int result;
    size_t position = 1;
    if (
        (pos2[0] == ':') &&
        (scanAndReadNDigitsInt(pos2, &position, sec, 2) == 0)
        )
    {
        result = 1;
    }
    else
    {
        result = EOF;
    }

    return result;

}

/*the following function does the same as  sscanf(pos2, "%d", &sec)*/
/*this function only exists because of optimizing valgrind time, otherwise sscanf would be just as good*/
static int sscanfd(const char *src, int* dst)
{
    int result;
    char* next;
    long int temp = strtol(src, &next, 10);
    if ((src == next) || (((temp == LONG_MAX) || (temp == LONG_MIN)) && (errno != 0)))
    {
        result = EOF;
    }
    else
    {
        (*dst) = (int)temp;
        result = 1;
    }
    return result;
}

/*the following function does the same as  sscanf(src, "%llu", &dst), but, it changes the src pointer.*/
static int sscanfllu(const char** src, unsigned long long* dst)
{
    int result = 1;
    char* next;
    (*dst) = strtoull((*src), &next, 10);
    if (((*src) == (const char*)next) || (((*dst) == ULLONG_MAX) && (errno != 0)))
    {
        result = EOF;
    }
    (*src) = (const char*)next;
    return result;
}

/*the following function does the same as  sscanf(src, ".%llu", &dst)*/
static int sscanfdotllu(const char*src, unsigned long long* dst)
{
    int result;

    if ((*src) != '.')
    {
        /*doesn't start with '.' error out*/
        result = EOF;
    }
    else
    {
        src++;
        result = sscanfllu(&src, dst);
    }

    return result;
}

/*the following function does the same as  sscanf(src, "%u", &dst)*/
static int sscanfu(const char* src, uint32_t* dst)
{
    int result;
    char* next;
    unsigned long int temp = strtoul(src, &next, 10);
    if ((src == next) || ((temp == ULONG_MAX) && (errno != 0)))
    {
        result = EOF;
    }
    else
    {
        result = 1;
        (*dst) = (uint32_t)temp;
    }
    return result;
}

/*the following function does the same as  sscanf(src, "%f", &dst)*/
static int sscanff(const char*src, float* dst)
{
    int result = 1;
    char* next;
    (*dst) = strtof(src, &next);
    if ((src == next) || (((*dst) == HUGE_VALF) && (errno != 0)))
    {
        result = EOF;
    }
    return result;
}

/*the following function does the same as  sscanf(src, "%lf", &dst)*/
static int sscanflf(const char*src, double* dst)
{
    int result = 1;
    char* next;
    (*dst) = strtod(src, &next);
    if ((src == next) || (((*dst) == HUGE_VALL) && (errno != 0)))
    {
        result = EOF;
    }
    return result;
}


/*the below function replaces sscanf(src, "%03d:%02d\"", &hourOffset, &minOffset)*/
/*return 2 if success*/

static int sscanf3d2d(const char* src, int* hourOffset, int* minOffset)
{
    size_t position = 0;
    int result = EOF;
    bool isNegative = false;

    if (*src == '+')
    {
        position++;
    }
    else if (*src == '-')
    {
        isNegative = true;
        position++;
    }

    if (
        (scanAndReadNDigitsInt(src, &position, hourOffset, (3-position)) == 0) &&
        (src[position++] == ':') &&
        (scanAndReadNDigitsInt(src, &position, minOffset, 2) == 0)
        )
    {
        result = 2;
    }
    else
    {
        result = 0;
    }
    if (isNegative)
        *hourOffset *= -1;

    return result;
}

AGENT_DATA_TYPES_RESULT CreateAgentDataType_From_String(const char* source, AGENT_DATA_TYPE_TYPE type, AGENT_DATA_TYPE* agentData)
{

    AGENT_DATA_TYPES_RESULT result;

    /* Codes_SRS_AGENT_TYPE_SYSTEM_99_073:[ CreateAgentDataType_From_String shall return AGENT_DATA_TYPES_INVALID_ARG if source is NULL.] */
    if ((source == NULL) ||
        /* Codes_SRS_AGENT_TYPE_SYSTEM_99_074:[ CreateAgentDataType_From_String shall return AGENT_DATA_TYPES_INVALID_ARG if agentData is NULL.] */
        (agentData == NULL))
    {
        result = AGENT_DATA_TYPES_INVALID_ARG;
        LogError("(result = %s)", MU_ENUM_TO_STRING(AGENT_DATA_TYPES_RESULT, result));
    }
    else
    {
        /* Codes_SRS_AGENT_TYPE_SYSTEM_99_071:[ CreateAgentDataType_From_String shall create an AGENT_DATA_TYPE from a char* representation of the type indicated by type parameter.] */
        /* Codes_SRS_AGENT_TYPE_SYSTEM_99_072:[ The implementation for the transformation of the char* source into AGENT_DATA_TYPE shall be type specific.] */
        switch (type)
        {
            default:
                /* Codes_SRS_AGENT_TYPE_SYSTEM_99_075:[ CreateAgentDataType_From_String shall return AGENT_DATA_TYPES_NOT_IMPLEMENTED if type is not a supported type.] */
                result = AGENT_DATA_TYPES_NOT_IMPLEMENTED;
                LogError("(result = %s)", MU_ENUM_TO_STRING(AGENT_DATA_TYPES_RESULT, result));
                break;

            /* Codes_SRS_AGENT_TYPE_SYSTEM_99_086:[ EDM_STRING] */
            case EDM_BOOLEAN_TYPE:
            {
                if (strcmp(source, "true") == 0)
                {
                    agentData->type = EDM_BOOLEAN_TYPE;
                    agentData->value.edmBoolean.value = EDM_TRUE;
                    result = AGENT_DATA_TYPES_OK;
                }
                else if (strcmp(source, "false") == 0)
                {
                    agentData->type = EDM_BOOLEAN_TYPE;
                    agentData->value.edmBoolean.value = EDM_FALSE;
                    result = AGENT_DATA_TYPES_OK;
                }
                else
                {
                    /* Codes_SRS_AGENT_TYPE_SYSTEM_99_087:[ CreateAgentDataType_From_String shall return AGENT_DATA_TYPES_INVALID_ARG if source is not a valid string for a value of type type.] */
                    result = AGENT_DATA_TYPES_INVALID_ARG;
                    LogError("(result = %s)", MU_ENUM_TO_STRING(AGENT_DATA_TYPES_RESULT, result));
                }

                break;
            }
            case EDM_NULL_TYPE:
            {
                if (strcmp(source, "null") == 0)
                {
                    agentData->type = EDM_NULL_TYPE;
                    result = AGENT_DATA_TYPES_OK;
                }
                else
                {
                    /* Codes_SRS_AGENT_TYPE_SYSTEM_99_087:[ CreateAgentDataType_From_String shall return AGENT_DATA_TYPES_INVALID_ARG if source is not a valid string for a value of type type.] */
                    result = AGENT_DATA_TYPES_INVALID_ARG;
                    LogError("(result = %s)", MU_ENUM_TO_STRING(AGENT_DATA_TYPES_RESULT, result));
                }

                break;
            }

            /* Codes_SRS_AGENT_TYPE_SYSTEM_99_084:[ EDM_SBYTE] */
            case EDM_SBYTE_TYPE:
            {
                int sByteValue;
                if ((sscanfd(source, &sByteValue) != 1) ||
                    (sByteValue < -128) ||
                    (sByteValue > 127))
                {
                    /* Codes_SRS_AGENT_TYPE_SYSTEM_99_087:[ CreateAgentDataType_From_String shall return AGENT_DATA_TYPES_INVALID_ARG if source is not a valid string for a value of type type.] */
                    result = AGENT_DATA_TYPES_INVALID_ARG;
                    LogError("(result = %s)", MU_ENUM_TO_STRING(AGENT_DATA_TYPES_RESULT, result));
                }
                else
                {
                    agentData->type = EDM_SBYTE_TYPE;
                    agentData->value.edmSbyte.value = (int8_t)sByteValue;
                    result = AGENT_DATA_TYPES_OK;
                }

                break;
            }

            /* Codes_SRS_AGENT_TYPE_SYSTEM_99_077:[ EDM_BYTE] */
            case EDM_BYTE_TYPE:
            {
                int byteValue;
                if ((sscanfd(source, &byteValue) != 1) ||
                    (byteValue < 0) ||
                    (byteValue > 255))
                {
                    /* Codes_SRS_AGENT_TYPE_SYSTEM_99_087:[ CreateAgentDataType_From_String shall return AGENT_DATA_TYPES_INVALID_ARG if source is not a valid string for a value of type type.] */
                    result = AGENT_DATA_TYPES_INVALID_ARG;
                    LogError("(result = %s)", MU_ENUM_TO_STRING(AGENT_DATA_TYPES_RESULT, result));
                }
                else
                {
                    agentData->type = EDM_BYTE_TYPE;
                    agentData->value.edmByte.value = (uint8_t)byteValue;
                    result = AGENT_DATA_TYPES_OK;
                }

                break;
            }

            /* Codes_SRS_AGENT_TYPE_SYSTEM_99_081:[ EDM_INT16] */
            case EDM_INT16_TYPE:
            {
                int int16Value;
                if ((sscanfd(source, &int16Value) != 1) ||
                    (int16Value < -32768) ||
                    (int16Value > 32767))
                {
                    /* Codes_SRS_AGENT_TYPE_SYSTEM_99_087:[ CreateAgentDataType_From_String shall return AGENT_DATA_TYPES_INVALID_ARG if source is not a valid string for a value of type type.] */
                    result = AGENT_DATA_TYPES_INVALID_ARG;
                    LogError("(result = %s)", MU_ENUM_TO_STRING(AGENT_DATA_TYPES_RESULT, result));
                }
                else
                {
                    agentData->type = EDM_INT16_TYPE;
                    agentData->value.edmInt16.value = (int16_t)int16Value;
                    result = AGENT_DATA_TYPES_OK;
                }

                break;
            }

            /* Codes_SRS_AGENT_TYPE_SYSTEM_99_082:[ EDM_INT32] */
            case EDM_INT32_TYPE:
            {
                int32_t int32Value;
                unsigned char isNegative;
                uint32_t uint32Value;
                const char* pos;
                size_t strLength;

                if (source[0] == '-')
                {
                    isNegative = 1;
                    pos = &source[1];
                }
                else
                {
                    isNegative = 0;
                    pos = &source[0];
                }

                strLength = strlen(source);

                if ((sscanfu(pos, &uint32Value) != 1) ||
                    (strLength > 11) ||
                    ((uint32Value > 2147483648UL) && isNegative) ||
                    ((uint32Value > 2147483647UL) && (!isNegative)))
                {
                    /* Codes_SRS_AGENT_TYPE_SYSTEM_99_087:[ CreateAgentDataType_From_String shall return AGENT_DATA_TYPES_INVALID_ARG if source is not a valid string for a value of type type.] */
                    result = AGENT_DATA_TYPES_INVALID_ARG;
                    LogError("(result = %s)", MU_ENUM_TO_STRING(AGENT_DATA_TYPES_RESULT, result));
                }
                else
                {
                    if (isNegative)
                    {
                        if (uint32Value == 2147483648UL)
                        {
                           int32Value = -2147483647L - 1L;
                        }
                        else
                        {
                            int32Value = -(int32_t)uint32Value;
                        }
                    }
                    else
                    {
                        int32Value = uint32Value;
                    }

                    agentData->type = EDM_INT32_TYPE;
                    agentData->value.edmInt32.value = (int32_t)int32Value;
                    result = AGENT_DATA_TYPES_OK;
                }

                break;
            }

            /* Codes_SRS_AGENT_TYPE_SYSTEM_99_083:[ EDM_INT64] */
            case EDM_INT64_TYPE:
            {
                long long int64Value;
                unsigned char isNegative;
                unsigned long long ullValue;
                const char* pos;
                size_t strLength;

                if (source[0] == '-')
                {
                    isNegative = 1;
                    pos = &source[1];
                }
                else
                {
                    isNegative = 0;
                    pos = &source[0];
                }

                strLength = strlen(source);

                if ((sscanfllu(&pos, &ullValue) != 1) ||
                    (strLength > 20) ||
                    ((ullValue > 9223372036854775808ULL) && isNegative) ||
                    ((ullValue > 9223372036854775807ULL) && (!isNegative)))
                {
                    /* Codes_SRS_AGENT_TYPE_SYSTEM_99_087:[ CreateAgentDataType_From_String shall return AGENT_DATA_TYPES_INVALID_ARG if source is not a valid string for a value of type type.] */
                    result = AGENT_DATA_TYPES_INVALID_ARG;
                    LogError("(result = %s)", MU_ENUM_TO_STRING(AGENT_DATA_TYPES_RESULT, result));
                }
                else
                {
                    if (isNegative)
                    {
                        if (ullValue == 9223372036854775808ULL)
                        {
                            int64Value = -9223372036854775807LL - 1LL;
                        }
                        else
                        {
                            int64Value = -(long long)ullValue;
                        }
                    }
                    else
                    {
                        int64Value = ullValue;
                    }
                    agentData->type = EDM_INT64_TYPE;
                    agentData->value.edmInt64.value = (int64_t)int64Value;
                    result = AGENT_DATA_TYPES_OK;
                }

                break;
            }

            /* Codes_SRS_AGENT_TYPE_SYSTEM_99_085:[ EDM_DATE] */
            case EDM_DATE_TYPE:
            {
                int year;
                int month;
                int day;
                size_t strLength = strlen(source);

                if ((strLength < 2) ||
                    (source[0] != '"') ||
                    (source[strLength - 1] != '"'))
                {
                    /* Codes_SRS_AGENT_TYPE_SYSTEM_99_087:[ CreateAgentDataType_From_String shall return AGENT_DATA_TYPES_INVALID_ARG if source is not a valid string for a value of type type.] */
                    result = AGENT_DATA_TYPES_INVALID_ARG;
                    LogError("(result = %s)", MU_ENUM_TO_STRING(AGENT_DATA_TYPES_RESULT, result));
                }
                else
                {
                    size_t pos = 1;
                    int sign;
                    scanOptionalMinusSign(source, 2, &pos, &sign);

                    if ((scanAndReadNDigitsInt(source, &pos, &year, 4) != 0) ||
                        (source[pos++] != '-') ||
                        (scanAndReadNDigitsInt(source, &pos, &month, 2) != 0) ||
                        (source[pos++] != '-') ||
                        (scanAndReadNDigitsInt(source, &pos, &day, 2) != 0) ||
                        (Create_AGENT_DATA_TYPE_from_date(agentData, (int16_t)(sign*year), (uint8_t)month, (uint8_t)day) != AGENT_DATA_TYPES_OK))
                    {
                        /* Codes_SRS_AGENT_TYPE_SYSTEM_99_087:[ CreateAgentDataType_From_String shall return AGENT_DATA_TYPES_INVALID_ARG if source is not a valid string for a value of type type.] */
                        result = AGENT_DATA_TYPES_INVALID_ARG;
                        LogError("(result = %s)", MU_ENUM_TO_STRING(AGENT_DATA_TYPES_RESULT, result));
                    }
                    else
                    {
                        result = AGENT_DATA_TYPES_OK;
                    }
                }

                break;
            }

            /* Codes_SRS_AGENT_TYPE_SYSTEM_99_078:[ EDM_DATETIMEOFFSET] */
            case EDM_DATE_TIME_OFFSET_TYPE:
            {
                int year;
                int month;
                int day;
                int hour;
                int min;
                int sec = 0;
                int hourOffset;
                int minOffset;
                unsigned long long fractionalSeconds = 0;
                size_t strLength = strlen(source);

                agentData->value.edmDateTimeOffset.hasFractionalSecond = 0;
                agentData->value.edmDateTimeOffset.hasTimeZone = 0;
                /* The value of tm_isdst is positive if Daylight Saving Time is in effect, zero if Daylight
                   Saving Time is not in effect, and negative if the information is not available.*/
                agentData->value.edmDateTimeOffset.dateTime.tm_isdst = -1;

                if ((strLength < 2) ||
                    (source[0] != '"') ||
                    (source[strLength - 1] != '"'))
                {
                    /* Codes_SRS_AGENT_TYPE_SYSTEM_99_087:[ CreateAgentDataType_From_String shall return AGENT_DATA_TYPES_INVALID_ARG if source is not a valid string for a value of type type.] */
                    result = AGENT_DATA_TYPES_INVALID_ARG;
                    LogError("(result = %s)", MU_ENUM_TO_STRING(AGENT_DATA_TYPES_RESULT, result));
                }
                else
                {
                    size_t pos = 1;
                    int sign;
                    scanOptionalMinusSign(source, 2, &pos, &sign);

                    if ((scanAndReadNDigitsInt(source, &pos, &year, 4) != 0) ||
                        (source[pos++] != '-') ||
                        (scanAndReadNDigitsInt(source, &pos, &month, 2) != 0) ||
                        (source[pos++] != '-') ||
                        (scanAndReadNDigitsInt(source, &pos, &day, 2) != 0) ||
                        (source[pos++] != 'T') ||
                        (scanAndReadNDigitsInt(source, &pos, &hour, 2) != 0) ||
                        (source[pos++] != ':') ||
                        (scanAndReadNDigitsInt(source, &pos, &min, 2) != 0))
                    {
                        /* Codes_SRS_AGENT_TYPE_SYSTEM_99_087:[ CreateAgentDataType_From_String shall return AGENT_DATA_TYPES_INVALID_ARG if source is not a valid string for a value of type type.] */
                        result = AGENT_DATA_TYPES_INVALID_ARG;
                        LogError("(result = %s)", MU_ENUM_TO_STRING(AGENT_DATA_TYPES_RESULT, result));
                    }
                    else
                    {
                        const char* pos2;
                        year = year*sign;
                        if ((pos2 = strchr(source, ':')) == NULL)
                        {
                            /* Codes_SRS_AGENT_TYPE_SYSTEM_99_087:[ CreateAgentDataType_From_String shall return AGENT_DATA_TYPES_INVALID_ARG if source is not a valid string for a value of type type.] */
                            result = AGENT_DATA_TYPES_INVALID_ARG;
                            LogError("(result = %s)", MU_ENUM_TO_STRING(AGENT_DATA_TYPES_RESULT, result));
                        }
                        else
                        {
                            pos2 += 3;
                            if (*pos2 == ':')
                            {
                                if (sscanf2d(pos2, &sec) != 1)
                                {
                                    pos2 = NULL;
                                }
                                else
                                {
                                    pos2 += 3;
                                }
                            }

                            if ((pos2 != NULL) &&
                                (*pos2 == '.'))
                            {
                                if (sscanfdotllu(pos2, &fractionalSeconds) != 1)
                                {
                                    pos2 = NULL;
                                }
                                else
                                {
                                    pos2++;

                                    agentData->value.edmDateTimeOffset.hasFractionalSecond = 1;

                                    while ((*pos2 != '\0') &&
                                        (IS_DIGIT(*pos2)))
                                    {
                                        pos2++;
                                    }

                                    if (*pos2 == '\0')
                                    {
                                        pos2 = NULL;
                                    }
                                }
                            }

                            if (pos2 == NULL)
                            {
                                /* Codes_SRS_AGENT_TYPE_SYSTEM_99_087:[ CreateAgentDataType_From_String shall return AGENT_DATA_TYPES_INVALID_ARG if source is not a valid string for a value of type type.] */
                                result = AGENT_DATA_TYPES_INVALID_ARG;
                                LogError("(result = %s)", MU_ENUM_TO_STRING(AGENT_DATA_TYPES_RESULT, result));
                            }
                            else
                            {
                                hourOffset = 0;
                                minOffset = 0;

                                if (sscanf3d2d(pos2, &hourOffset, &minOffset) == 2)
                                {
                                    agentData->value.edmDateTimeOffset.hasTimeZone = 1;
                                }

                                if ((strcmp(pos2, "Z\"") == 0) ||
                                    agentData->value.edmDateTimeOffset.hasTimeZone)
                                {
                                    if ((ValidateDate(year, month, day) != 0) ||
                                        (hour < 0) ||
                                        (hour > 23) ||
                                        (min < 0) ||
                                        (min > 59) ||
                                        (sec < 0) ||
                                        (sec > 59) ||
                                        (fractionalSeconds > 999999999999) ||
                                        (hourOffset < -23) ||
                                        (hourOffset > 23) ||
                                        (minOffset < 0) ||
                                        (minOffset > 59))
                                    {
                                        /* Codes_SRS_AGENT_TYPE_SYSTEM_99_087:[ CreateAgentDataType_From_String shall return AGENT_DATA_TYPES_INVALID_ARG if source is not a valid string for a value of type type.] */
                                        result = AGENT_DATA_TYPES_INVALID_ARG;
                                        LogError("(result = %s)", MU_ENUM_TO_STRING(AGENT_DATA_TYPES_RESULT, result));
                                    }
                                    else
                                    {
                                        agentData->type = EDM_DATE_TIME_OFFSET_TYPE;
                                        agentData->value.edmDateTimeOffset.dateTime.tm_year= year-1900;
                                        agentData->value.edmDateTimeOffset.dateTime.tm_mon = month-1;
                                        agentData->value.edmDateTimeOffset.dateTime.tm_mday = day;
                                        agentData->value.edmDateTimeOffset.dateTime.tm_hour = hour;
                                        agentData->value.edmDateTimeOffset.dateTime.tm_min = min;
                                        agentData->value.edmDateTimeOffset.dateTime.tm_sec = sec;
                                        /*fill in tm_wday and tm_yday*/
                                        fill_tm_yday_and_tm_wday(&agentData->value.edmDateTimeOffset.dateTime);
                                        agentData->value.edmDateTimeOffset.fractionalSecond = (uint64_t)fractionalSeconds;
                                        agentData->value.edmDateTimeOffset.timeZoneHour = (int8_t)hourOffset;
                                        agentData->value.edmDateTimeOffset.timeZoneMinute = (uint8_t)minOffset;
                                        result = AGENT_DATA_TYPES_OK;
                                    }
                                }
                                else
                                {
                                    /* Codes_SRS_AGENT_TYPE_SYSTEM_99_087:[ CreateAgentDataType_From_String shall return AGENT_DATA_TYPES_INVALID_ARG if source is not a valid string for a value of type type.] */
                                    result = AGENT_DATA_TYPES_INVALID_ARG;
                                    LogError("(result = %s)", MU_ENUM_TO_STRING(AGENT_DATA_TYPES_RESULT, result));
                                }
                            }
                        }
                    }
                }

                break;
            }

            /* Codes_SRS_AGENT_TYPE_SYSTEM_99_080:[ EDM_DOUBLE] */
            case EDM_DOUBLE_TYPE:
            {
                if (strcmp(source, "\"NaN\"") == 0)
                {
                    agentData->type = EDM_DOUBLE_TYPE;
                    agentData->value.edmDouble.value = NAN;
                    result = AGENT_DATA_TYPES_OK;
                }
                else if (strcmp(source, "\"INF\"") == 0)
                {
                    agentData->type = EDM_DOUBLE_TYPE;
                    agentData->value.edmDouble.value = INFINITY;
                    result = AGENT_DATA_TYPES_OK;
                }
                else if (strcmp(source, "\"-INF\"") == 0)
                {
                    agentData->type = EDM_DOUBLE_TYPE;
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4056) /* Known warning for INIFNITY */
#endif
                    agentData->value.edmDouble.value = -INFINITY;
#ifdef _MSC_VER
#pragma warning(pop)
#endif
                    result = AGENT_DATA_TYPES_OK;
                }
                else if (sscanflf(source, &(agentData->value.edmDouble.value)) != 1)
                {
                    /* Codes_SRS_AGENT_TYPE_SYSTEM_99_087:[ CreateAgentDataType_From_String shall return AGENT_DATA_TYPES_INVALID_ARG if source is not a valid string for a value of type type.] */
                    result = AGENT_DATA_TYPES_INVALID_ARG;
                }
                else
                {
                    agentData->type = EDM_DOUBLE_TYPE;
                    result = AGENT_DATA_TYPES_OK;
                }
                break;
            }

            /* Codes_SRS_AGENT_TYPE_SYSTEM_99_089:[EDM_SINGLE] */
            case EDM_SINGLE_TYPE:
            {
                if (strcmp(source, "\"NaN\"") == 0)
                {
                    agentData->type = EDM_SINGLE_TYPE;
                    agentData->value.edmSingle.value = NAN;
                    result = AGENT_DATA_TYPES_OK;
                }
                else if (strcmp(source, "\"INF\"") == 0)
                {
                    agentData->type = EDM_SINGLE_TYPE;
                    agentData->value.edmSingle.value = INFINITY;
                    result = AGENT_DATA_TYPES_OK;
                }
                else if (strcmp(source, "\"-INF\"") == 0)
                {
                    agentData->type = EDM_SINGLE_TYPE;
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4056) /* Known warning for INIFNITY */
#endif
                    agentData->value.edmSingle.value = -INFINITY;
#ifdef _MSC_VER
#pragma warning(pop)
#endif
result = AGENT_DATA_TYPES_OK;
                }
                else if (sscanff(source, &agentData->value.edmSingle.value) != 1)
                {
                    /* Codes_SRS_AGENT_TYPE_SYSTEM_99_087:[ CreateAgentDataType_From_String shall return AGENT_DATA_TYPES_INVALID_ARG if source is not a valid string for a value of type type.] */
                    result = AGENT_DATA_TYPES_INVALID_ARG;
                    LogError("(result = %s)", MU_ENUM_TO_STRING(AGENT_DATA_TYPES_RESULT, result));
                }
                else
                {
                    agentData->type = EDM_SINGLE_TYPE;
                    result = AGENT_DATA_TYPES_OK;
                }
                break;
            }

            /* Codes_SRS_AGENT_TYPE_SYSTEM_99_079:[ EDM_DECIMAL] */
            case EDM_DECIMAL_TYPE:
            {
                size_t strLength = strlen(source);
                if ((strLength < 2) ||
                    (source[0] != '"') ||
                    (source[strLength - 1] != '"') ||
                    (ValidateDecimal(source + 1, strLength - 2) != 0))
                {
                    /* Codes_SRS_AGENT_TYPE_SYSTEM_99_087:[ CreateAgentDataType_From_String shall return AGENT_DATA_TYPES_INVALID_ARG if source is not a valid string for a value of type type.] */
                    result = AGENT_DATA_TYPES_INVALID_ARG;
                    LogError("(result = %s)", MU_ENUM_TO_STRING(AGENT_DATA_TYPES_RESULT, result));
                }
                else
                {
                    agentData->type = EDM_DECIMAL_TYPE;
                    agentData->value.edmDecimal.value = STRING_construct_n(source + 1, strLength-2);
                    if (agentData->value.edmDecimal.value == NULL)
                    {
                        /* Codes_SRS_AGENT_TYPE_SYSTEM_99_088:[ CreateAgentDataType_From_String shall return AGENT_DATA_TYPES_ERROR if any other error occurs.] */
                        result = AGENT_DATA_TYPES_ERROR;
                        LogError("(result = %s)", MU_ENUM_TO_STRING(AGENT_DATA_TYPES_RESULT, result));
                    }
                    else
                    {
                        result = AGENT_DATA_TYPES_OK;
                    }
                }
                break;
            }

            /* Codes_SRS_AGENT_TYPE_SYSTEM_99_086:[ EDM_STRING] */
            case EDM_STRING_TYPE:
            {
                size_t strLength = strlen(source);
                if ((strLength < 2) ||
                    (source[0] != '"') ||
                    (source[strLength - 1] != '"'))
                {
                    /* Codes_SRS_AGENT_TYPE_SYSTEM_99_087:[ CreateAgentDataType_From_String shall return AGENT_DATA_TYPES_INVALID_ARG if source is not a valid string for a value of type type.] */
                    result = AGENT_DATA_TYPES_INVALID_ARG;
                    LogError("(result = %s)", MU_ENUM_TO_STRING(AGENT_DATA_TYPES_RESULT, result));
                }
                else
                {
                    char* temp;
                    if ((temp = (char*)malloc(strLength - 1)) == NULL)
                    {
                        result = AGENT_DATA_TYPES_ERROR;
                        LogError("(result = %s)", MU_ENUM_TO_STRING(AGENT_DATA_TYPES_RESULT, result));
                    }
                    else if (memcpy(temp, source + 1, strLength - 2) == NULL)
                    {
                        free(temp);

                        result = AGENT_DATA_TYPES_ERROR;
                        LogError("(result = %s)", MU_ENUM_TO_STRING(AGENT_DATA_TYPES_RESULT, result));
                    }
                    else
                    {
                        // Removes the "
                        temp[strLength - 2] = 0;
                        agentData->type = EDM_STRING_TYPE;
                        agentData->value.edmString.chars = temp;
                        agentData->value.edmString.length = strLength - 2;
                        result = AGENT_DATA_TYPES_OK;
                    }
                }
                break;
            }
            /* Codes_SRS_AGENT_TYPE_SYSTEM_01_004: [EDM_STRING_NO_QUOTES] */
            case EDM_STRING_NO_QUOTES_TYPE:
            {
                char* temp;
                size_t strLength = strlen(source);
                if (mallocAndStrcpy_s(&temp, source) != 0)
                {
                    result = AGENT_DATA_TYPES_ERROR;
                    LogError("(result = %s)", MU_ENUM_TO_STRING(AGENT_DATA_TYPES_RESULT, result));
                }
                else
                {
                    agentData->type = EDM_STRING_NO_QUOTES_TYPE;
                    agentData->value.edmStringNoQuotes.chars = temp;
                    agentData->value.edmStringNoQuotes.length = strLength;
                    result = AGENT_DATA_TYPES_OK;
                }
                break;
            }
            /*Codes_SRS_AGENT_TYPE_SYSTEM_99_097:[ EDM_GUID]*/
            case EDM_GUID_TYPE:
            {
                if (strlen(source) != GUID_STRING_LENGTH)
                {
                    result = AGENT_DATA_TYPES_INVALID_ARG;
                }
                else
                {
                    if (source[0] != '"')
                    {
                        result = AGENT_DATA_TYPES_INVALID_ARG;
                    }
                    else if (scanMandatory2CapitalHexDigits(source + 1, &(agentData->value.edmGuid.GUID[0])) != 0)
                    {
                        result = AGENT_DATA_TYPES_INVALID_ARG;
                    }
                    else if (scanMandatory2CapitalHexDigits(source + 3, &(agentData->value.edmGuid.GUID[1])) != 0)
                    {
                        result = AGENT_DATA_TYPES_INVALID_ARG;
                    }
                    else if (scanMandatory2CapitalHexDigits(source + 5, &(agentData->value.edmGuid.GUID[2])) != 0)
                    {
                        result = AGENT_DATA_TYPES_INVALID_ARG;
                    }
                    else if (scanMandatory2CapitalHexDigits(source + 7, &(agentData->value.edmGuid.GUID[3])) != 0)
                    {
                        result = AGENT_DATA_TYPES_INVALID_ARG;
                    }
                    else if (source[9] != '-')
                    {
                        result = AGENT_DATA_TYPES_INVALID_ARG;
                    }
                    else if (scanMandatory2CapitalHexDigits(source + 10, &(agentData->value.edmGuid.GUID[4])) != 0)
                    {
                        result = AGENT_DATA_TYPES_INVALID_ARG;
                    }
                    else if (scanMandatory2CapitalHexDigits(source + 12, &(agentData->value.edmGuid.GUID[5])) != 0)
                    {
                        result = AGENT_DATA_TYPES_INVALID_ARG;
                    }
                    else if (source[14] != '-')
                    {
                        result = AGENT_DATA_TYPES_INVALID_ARG;
                    }
                    else if (scanMandatory2CapitalHexDigits(source + 15, &(agentData->value.edmGuid.GUID[6])) != 0)
                    {
                        result = AGENT_DATA_TYPES_INVALID_ARG;
                    }
                    else if (scanMandatory2CapitalHexDigits(source + 17, &(agentData->value.edmGuid.GUID[7])) != 0)
                    {
                        result = AGENT_DATA_TYPES_INVALID_ARG;
                    }
                    else if (source[19] != '-')
                    {
                        result = AGENT_DATA_TYPES_INVALID_ARG;
                    }
                    else if (scanMandatory2CapitalHexDigits(source + 20, &(agentData->value.edmGuid.GUID[8])) != 0)
                    {
                        result = AGENT_DATA_TYPES_INVALID_ARG;
                    }
                    else if (scanMandatory2CapitalHexDigits(source + 22, &(agentData->value.edmGuid.GUID[9])) != 0)
                    {
                        result = AGENT_DATA_TYPES_INVALID_ARG;
                    }
                    else if (source[24] != '-')
                    {
                        result = AGENT_DATA_TYPES_INVALID_ARG;
                    }
                    else if (scanMandatory2CapitalHexDigits(source + 25, &(agentData->value.edmGuid.GUID[10])) != 0)
                    {
                        result = AGENT_DATA_TYPES_INVALID_ARG;
                    }
                    else if (scanMandatory2CapitalHexDigits(source + 27, &(agentData->value.edmGuid.GUID[11])) != 0)
                    {
                        result = AGENT_DATA_TYPES_INVALID_ARG;
                    }
                    else if (scanMandatory2CapitalHexDigits(source + 29, &(agentData->value.edmGuid.GUID[12])) != 0)
                    {
                        result = AGENT_DATA_TYPES_INVALID_ARG;
                    }
                    else if (scanMandatory2CapitalHexDigits(source + 31, &(agentData->value.edmGuid.GUID[13])) != 0)
                    {
                        result = AGENT_DATA_TYPES_INVALID_ARG;
                    }
                    else if (scanMandatory2CapitalHexDigits(source + 33, &(agentData->value.edmGuid.GUID[14])) != 0)
                    {
                        result = AGENT_DATA_TYPES_INVALID_ARG;
                    }
                    else if (scanMandatory2CapitalHexDigits(source + 35, &(agentData->value.edmGuid.GUID[15])) != 0)
                    {
                        result = AGENT_DATA_TYPES_INVALID_ARG;
                    }
                    else if (source[37] != '"')
                    {
                        result = AGENT_DATA_TYPES_INVALID_ARG;
                    }
                    else
                    {
                        agentData->type = EDM_GUID_TYPE;
                        result = AGENT_DATA_TYPES_OK;
                    }
                }
                break;
            }
            case EDM_BINARY_TYPE:
            {
                size_t sourceLength = strlen(source);
                if (sourceLength < 2)
                {
                    result = AGENT_DATA_TYPES_INVALID_ARG;
                }
                else if (sourceLength == 2)
                {
                    agentData->type = EDM_BINARY_TYPE;
                    agentData->value.edmBinary.data = NULL;
                    agentData->value.edmBinary.size = 0;
                    result = AGENT_DATA_TYPES_OK;
                }
                else
                {
                    size_t sourcePosition = 0;
                    if (source[sourcePosition++] != '"') /*if it doesn't start with a quote then... */
                    {
                        result = AGENT_DATA_TYPES_INVALID_ARG;
                    }
                    else
                    {
                        /*1. read groups of 4 base64 character and transfer those into groups of 3 "normal" characters.
                        2. read the end of the string and produce from that the ending characters*/

                        /*compute the amount of memory to allocate*/
                        agentData->value.edmBinary.size = (((sourceLength - 2) + 4) / 4) * 3; /*this is overallocation, shall be trimmed later*/
                        agentData->value.edmBinary.data = (unsigned char*)malloc(agentData->value.edmBinary.size); /*this is overallocation, shall be trimmed later*/
                        if (agentData->value.edmBinary.data == NULL)
                        {
                            result = AGENT_DATA_TYPES_ERROR;
                        }
                        else
                        {

                            size_t destinationPosition = 0;
                            size_t consumed;
                            /*read and store "solid" groups of 4 base64 chars*/
                            while (scan4base64char(source + sourcePosition, sourceLength - sourcePosition, agentData->value.edmBinary.data + destinationPosition, agentData->value.edmBinary.data + destinationPosition + 1, agentData->value.edmBinary.data + destinationPosition + 2) == 0)
                            {
                                sourcePosition += 4;
                                destinationPosition += 3;
                            }

                            if (scanbase64b16(source + sourcePosition, sourceLength - sourcePosition, &consumed, agentData->value.edmBinary.data + destinationPosition, agentData->value.edmBinary.data + destinationPosition + 1) == 0)
                            {
                                sourcePosition += consumed;
                                destinationPosition += 2;

                            }
                            else if (scanbase64b8(source + sourcePosition, sourceLength - sourcePosition, &consumed, agentData->value.edmBinary.data + destinationPosition) == 0)
                            {
                                sourcePosition += consumed;
                                destinationPosition += 1;
                            }

                            if (source[sourcePosition++] != '"') /*if it doesn't end with " then bail out*/
                            {
                                free(agentData->value.edmBinary.data);
                                agentData->value.edmBinary.data = NULL;
                                result = AGENT_DATA_TYPES_INVALID_ARG;
                            }
                            else if (sourcePosition != sourceLength)
                            {
                                free(agentData->value.edmBinary.data);
                                agentData->value.edmBinary.data = NULL;
                                result = AGENT_DATA_TYPES_INVALID_ARG;
                            }
                            else
                            {
                                /*trim the result*/
                                void* temp = realloc(agentData->value.edmBinary.data, destinationPosition);
                                if (temp == NULL) /*this is extremely unlikely to happen, but whatever*/
                                {
                                    free(agentData->value.edmBinary.data);
                                    agentData->value.edmBinary.data = NULL;
                                    result = AGENT_DATA_TYPES_ERROR;
                                }
                                else
                                {
                                    agentData->type = EDM_BINARY_TYPE;
                                    agentData->value.edmBinary.data = (unsigned char*)temp;
                                    agentData->value.edmBinary.size = destinationPosition;
                                    result = AGENT_DATA_TYPES_OK;
                                }
                            }
                        }
                    }
                }
                break;
            }
        }
    }

    return result;
}

// extern AGENT_DATA_TYPES_RESULT AgentDataType_GetComplexTypeField(AGENT_DATA_TYPE* agentData, size_t index, COMPLEX_TYPE_FIELD_TYPE* complexField);
COMPLEX_TYPE_FIELD_TYPE* AgentDataType_GetComplexTypeField(AGENT_DATA_TYPE* agentData, size_t index)
{
    COMPLEX_TYPE_FIELD_TYPE* complexField = NULL;
    if (agentData == NULL)
    {
        LogError("(result = %s)", MU_ENUM_TO_STRING(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG));
    }
    else
    {
        if (agentData->type != EDM_COMPLEX_TYPE_TYPE)
        {
            LogError("(result = %s)", MU_ENUM_TO_STRING(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG));
        }

        else
        {
            if (index >= agentData->value.edmComplexType.nMembers)
            {
                LogError("(result = %s)", MU_ENUM_TO_STRING(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG));
            }
            else
            {
                complexField = (COMPLEX_TYPE_FIELD_TYPE*)malloc(sizeof(COMPLEX_TYPE_FIELD_TYPE));
                if (complexField == NULL)
                {
                    LogError("(result = %s)", MU_ENUM_TO_STRING(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_ERROR));
                }
                else
                {
                    *complexField = agentData->value.edmComplexType.fields[index];
                }
            }
        }
    }
    return complexField;
}

