// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef AGENT_DATA_TYPES_H
#define AGENT_DATA_TYPES_H

#ifdef __cplusplus
#include <cstdint>
#include <ctime>
#include <cstddef>
#else
#if ((defined _WIN32_WCE) && _WIN32_WCE==0x0600)
#include "stdint_ce6.h"
#else
#include <stdint.h>
#endif
#include <stddef.h>
#endif

#include "azure_c_shared_utility/agenttime.h"
#include "azure_macro_utils/macro_utils.h"
#include "azure_c_shared_utility/strings.h"

/*Codes_SRS_AGENT_TYPE_SYSTEM_99_001:[ AGENT_TYPE_SYSTEM shall have the following interface]*/

#ifdef __cplusplus
extern "C"
{
#endif

/*the following forward declarations the closest implementation to "interface" in OOP*/
struct AGENT_DATA_TYPE_TAG;
typedef struct AGENT_DATA_TYPE_TAG AGENT_DATA_TYPE;

/*this file contains the definitions of the data types of EDM*/
/*the types are taken from */
/*http://docs.oasis-open.org/odata/odata/v4.0/cos01/part3-csdl/odata-v4.0-cos01-part3-csdl.html*/
/*chapter 4.4 - "Primitive Data Types" */

/*the C implementation of these types follows:*/
/*even the simpler types are encapsulated in structs to purposely avoid compiler promotions/casts etc*/

/*Binary data.*/
typedef struct EDM_BINARY_TAG
{
    size_t size;
    unsigned char* data;
}EDM_BINARY;

#define EDM_BOOLEANS_VALUES \
    EDM_TRUE, \
    EDM_FALSE

MU_DEFINE_ENUM_WITHOUT_INVALID(EDM_BOOLEANS, EDM_BOOLEANS_VALUES);

/*ispositiveinfinity*/

#ifdef _MSC_VER
#define ISPOSITIVEINFINITY(x) ((_finite((x))==0) && ((_fpclass((x)) & _FPCLASS_PINF) == _FPCLASS_PINF))
#else
#if defined __STDC_VERSION__
#if ((__STDC_VERSION__  == 199901L) || (__STDC_VERSION__ == 201112L) || (__STDC_VERSION__ == 201710L))
/*C99, C11 or C18 compiler*/
#define ISPOSITIVEINFINITY(x) (isinf((x)) && (signbit((x))==0))
#else
#error update this file to contain the latest C standard.
#endif
#else
#ifdef __cplusplus
#define ISPOSITIVEINFINITY(x) (std::isinf((x)) && (signbit((x))==0))
#else
#error unknown (or C89) compiler, must provide a definition for ISPOSITIVEINFINITY
#endif
#endif
#endif

#ifdef _MSC_VER
/*not exactly signbit*/
#define ISNEGATIVEINFINITY(x) ((_finite((x))==0) && ((_fpclass((x)) & _FPCLASS_NINF) == _FPCLASS_NINF))
#else
#if defined __STDC_VERSION__
#if ((__STDC_VERSION__  == 199901L) || (__STDC_VERSION__ == 201112L) || (__STDC_VERSION__ == 201710L))
/*C99, C11 or C18 compiler*/
#define ISNEGATIVEINFINITY(x) (isinf((x)) && (signbit((x))!=0))
#else
#error update this file to contain the latest C standard.
#endif
#else
#ifdef __cplusplus
#define ISNEGATIVEINFINITY(x) (std::isinf((x)) && (signbit((x)) != 0))
#else
#error unknown (or C89) compiler, must provide a definition for ISNEGATIVEINFINITY
#endif
#endif
#endif

/*Binary-valued logic.*/
typedef struct EDM_BOOLEAN_TAG
{
    EDM_BOOLEANS value;
}EDM_BOOLEAN;

/*Unsigned 8-bit integer*/
typedef struct EDM_BYTE_TAG
{
    uint8_t value;
} EDM_BYTE;

/*Date without a time-zone offset*/
/*The edm:Date expression evaluates to a primitive date value. A date expression MUST be assigned a
value of type xs:date, see [XML-Schema-2], section 3.3.9. The value MUST also conform to rule
dateValue in [OData-ABNF], i.e. it MUST NOT contain a time-zone offset.*/
/*section 3.3.9: date uses the date/timeSevenPropertyModel, with hour, minute, and second required to be absent.*/
/*dateValue in OData-ABNF is : dateValue = year "-" month "-" day */
/*year  = [ "-" ] ( "0" 3DIGIT / oneToNine 3*DIGIT )
month = "0" oneToNine
/ "1" ( "0" / "1" / "2" )
day   = "0" oneToNine
/ ( "1" / "2" ) DIGIT
/ "3" ( "0" / "1" )*/
typedef struct EDM_DATE_TAG
{
    int16_t year; /*can represent all values for a year*/ /*they can be between -9999 and 9999*/
    uint8_t month;
    uint8_t day;
} EDM_DATE;


/*The edm:DateTimeOffset expression evaluates to a primitive date/time value with a time-zone offset.
A date/time expression MUST be assigned a value of type xs:dateTimeStamp, see [XML-Schema-2],
section 3.4.28. The value MUST also conform to rule dateTimeOffsetValue in [OData-ABNF], i.e. it
MUST NOT contain an end-of-day fragment (24:00:00).*/
/*section 3.4.28 says : dateTimeStampLexicalRep ::= yearFrag '-' monthFrag '-' dayFrag 'T' ((hourFrag ':' minuteFrag ':' secondFrag) | endOfDayFrag) timezoneFrag?*/
/*[OData-ABNF] says: dateTimeOffsetValue = year "-" month "-" day "T" hour ":" minute [ ":" second [ "." fractionalSeconds ] ] ( "Z" / sign hour ":" minute )*/
/*fractionalSeconds = 1*12DIGIT, FYI*/
typedef struct EDM_DATE_TIME_OFFSET_TAG
{
    struct tm dateTime;
    uint8_t hasFractionalSecond;
    uint64_t fractionalSecond; /*because UINT32 only has 10 digits*/
    uint8_t hasTimeZone;
    int8_t timeZoneHour;
    uint8_t timeZoneMinute;
}EDM_DATE_TIME_OFFSET;

/*Edm.Guid*/
/*16-byte (128-bit) unique identifier*/
/*The edm:Guid expression evaluates to a primitive 32-character string value. A guid expression MUST be
assigned a value conforming to the rule guidValue in [OData-ABNF].*/
/*guidValue is 8HEXDIG "-" 4HEXDIG "-" 4HEXDIG "-" 4HEXDIG "-" 12HEXDIG*/
typedef struct EDM_GUID_TAG
{
    uint8_t GUID[16];
}EDM_GUID;


/*Edm.Decimal*/
/*The edm:Decimal expression evaluates to a primitive decimal value. A decimal expression MUST be
assigned a value conforming to the rule decimalValue in [OData-ABNF].*/
/*[OData-ABNF] says: decimalValue = [SIGN] 1*DIGIT ["." 1*DIGIT] */
/* this is binary coded decimal style then*/
typedef struct EDM_DECIMAL_TAG
{
    STRING_HANDLE value;
} EDM_DECIMAL;

/*Edm.Double*/
/*IEEE 754 binary64 floating-point number (15-17 decimal digits)*/

typedef struct EDM_DOUBLE_TAG
{
    double value;
} EDM_DOUBLE;


/*Edm.Duration*/
/*Signed duration in days, hours, minutes, and (sub)seconds*/
/*The edm:Duration expression evaluates to a primitive duration value. A duration expression MUST be
assigned a value of type xs:dayTimeDuration, see [XML-Schema-2], section 3.4.27.*/
/*XML-Schema section 3.4.27 says: "[...]leaving only those with day, hour, minutes, and/or seconds fields." */
/*day is "unsignedNoDecimalPtNumeral" and that can have as many digits... ?*/
typedef struct EDM_DURATION_TAG
{
    size_t nDigits;
    char* digits;
}
EDM_DURATION;


/*Edm.Int16*/
/*Signed 16-bit integer*/
/*[OData=ABNF] says about Int16:  numbers in the range from -32768 to 32767 */
/*this is not C compliant, because C89 has for (read:guarantees) short SHRT_MIN -32767... (notice how it misses -32768)*/
/*C99 has the same*/
/*C11 has the same*/
/*platform types has to check for -32768 compliance*/
typedef struct EDM_INT16_TAG
{
    int16_t value;
} EDM_INT16;

/*Edm.Int32*/
/*Signed 32-bit integer*/
/*OData-ABNF has for int32Value = [ sign ] 1*10DIGIT ; numbers in the range from -2147483648 to 2147483647*/
/*same issue as for EDM_16*/
/*platform types has to check compliance based on LONG_MIN #define*/
typedef struct EDM_INT32_TAG
{
    int32_t value;
} EDM_INT32;

/*Edm.Int64*/
/*Signed 64-bit integer*/
/*OData=ABNF: int64Value = [ sign ] 1*19DIGIT ; numbers in the range from -9223372036854775808 to 9223372036854775807*/
/*C89 has no mention of anything on 64bits*/
/*C99 mention LLONG_MIN as -9223372036854775807 and LLONG_MAX as 9223372036854775807*/
/*C11 is the same as C99*/
typedef struct EDM_INT64_TAG
{
    int64_t value; /*SINT64 might be a single type or s truct provided by platformTypes, depending on C compiler support*/
} EDM_INT64;

/*Edm.SByte*/
/*Signed 8-bit integer*/
/*OData=ABNF: sbyteValue = [ sign ] 1*3DIGIT  ; numbers in the range from -128 to 127*/
/*C89, C99, C11 all have SCHAR_MIN, SCHAR_MAX between -127 and 127 (guaranteed)*/
/*so platformTypes.h has to check that -128 is attainable*/
typedef struct EDM_SBYTE_TAG
{
    int8_t value;
} EDM_SBYTE;

/*Edm.Single*/
/*IEEE 754 binary32 floating-point number (6-9 decimal digits)*/
/*with the same "fears" as for Edm.Double*/
typedef struct EDM_SINGLE_TAG
{
    float value;
} EDM_SINGLE;

/*not clear what this is
typedef EDM_STREAM_TAG
{

}EDM_STREAM;
*/

/*Edm.String*/
/*Sequence of UTF-8 characters*/
typedef struct EDM_STRING_TAG
{
    size_t length; /*number of unsigned char* in the string*/
    char* chars;
} EDM_STRING;

/*Edm.TimeOfDay*/
/*Clock time 00:00-23:59:59.999999999999*/
/*The edm:TimeOfDay expression evaluates to a primitive time value. A time-of-day expression MUST be
assigned a value conforming to the rule timeOfDayValue in [OData-ABNF].*/
/*timeOfDayValue = hour ":" minute [ ":" second [ "." fractionalSeconds ] ]*/
typedef struct EDM_TIME_OF_DAY_TAG
{
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
    uint64_t fractionalSecond;
}EDM_TIME_OF_DAY;

/*positionLiteral  = doubleValue SP doubleValue  ; longitude, then latitude*/
typedef struct EDM_POSITION_LITERAL_TAG
{
    double longitude;
    double latitude;
}EDM_POSITION_LITERAL;


/*sridLiteral      = "SRID" EQ 1*5DIGIT SEMI*/
typedef struct EDM_SRID_LITERAL_TAG
{
    uint16_t digits;
} EDM_SRID_LITERAL;

/*lineStringData        = OPEN positionLiteral 1*( COMMA positionLiteral ) CLOSE*/
typedef struct EDM_LINE_STRING_DATA_TAG
{
    size_t nPositionLiterals;
    EDM_POSITION_LITERAL *positionLiterals;
}EDM_LINE_STRING_DATA;

/*pointData        = OPEN positionLiteral CLOSE*/
typedef struct EDM_POINT_DATA_TAG
{
    EDM_POSITION_LITERAL positionLiteral;
}EDM_POINT_DATA;

/*ringLiteral        = OPEN positionLiteral *( COMMA positionLiteral ) CLOSE*/
typedef struct EDM_RING_LITERAL_TAG
{
    size_t nPositionLiterals;
    EDM_POSITION_LITERAL *positionLiterals;
}EDM_RING_LITERAL;

/*pointLiteral     ="Point" pointData*/
typedef struct EDM_POINT_LITERAL_TAG
{
    EDM_POINT_DATA pointData;
} EDM_POINT_LITERAL;

/*polygonData        = OPEN ringLiteral *( COMMA ringLiteral ) CLOSE*/
typedef struct EDM_POLYGON_DATA_TAG
{
    size_t nRingLiterals;
    EDM_RING_LITERAL *ringLiterals;
}EDM_POLYGON_DATA;

/*polygonLiteral     = "Polygon" polygonData*/
typedef struct EDM_POLYGON_LITERAL_TAG
{
    EDM_POLYGON_DATA polygonData;
}EDM_POLYGON_LITERAL;

/*fullPolygonLiteral = sridLiteral polygonLiteral*/
typedef struct EDM_FULL_POLYGON_LITERAL_TAG
{
    EDM_SRID_LITERAL srid;
    EDM_POLYGON_LITERAL polygonLiteral;
}EDM_FULL_POLYGON_LITERAL;

/*fullPointLiteral = sridLiteral pointLiteral*/
typedef struct EDM_FULL_POINT_LITERAL_TAG
{
    EDM_SRID_LITERAL srid;
    EDM_POINT_LITERAL pointLiteral;
} EDM_FULL_POINT_LITERAL;

/*geographyPoint   = geographyPrefix SQUOTE fullPointLiteral SQUOTE*/
typedef struct EDM_GEOGRAPHY_POINT_TAG
{
    EDM_FULL_POINT_LITERAL fullPointLiteral;
}EDM_GEOGRAPHY_POINT;

/*multiPolygonLiteral     = "MultiPolygon(" [ polygonData *( COMMA polygonData ) ] CLOSE*/
typedef struct EDM_MULTI_POLYGON_LITERAL_TAG
{
    size_t nPolygonDatas;
    EDM_POLYGON_DATA * polygonDatas;
}EDM_MULTI_POLYGON_LITERAL;

/*fullMultiPolygonLiteral = sridLiteral multiPolygonLiteral*/
typedef struct EDM_FULL_MULTI_POLYGON_LITERAL_TAG
{
    EDM_SRID_LITERAL srid;
    EDM_MULTI_POLYGON_LITERAL multiPolygonLiteral;
}EDM_FULL_MULTI_POLYGON_LITERAL;

/*multiPointLiteral     = "MultiPoint(" [ pointData *( COMMA pointData ) ] CLOSE*/
typedef struct EDM_MULTI_POINT_LITERAL_TAG
{
    size_t nPointDatas;
    EDM_POINT_DATA *pointDatas;
}EDM_MULTI_POINT_LITERAL;

/*fullMultiPointLiteral = sridLiteral multiPointLiteral*/
typedef struct EDM_FULL_MULTI_POINT_LITERAL_TAG
{
    EDM_SRID_LITERAL srid;
    EDM_MULTI_POINT_LITERAL multiPointLiteral;
}EDM_FULL_MULTI_POINT_LITERAL;

/*lineStringLiteral     = "LineString" lineStringData*/
typedef struct EDM_LINE_STRING_LITERAL_TAG
{
    EDM_LINE_STRING_DATA lineStringData;
}EDM_LINE_STRING_LITERAL;

/*fullLineStringLiteral = sridLiteral lineStringLiteral*/
typedef struct EDM_FULL_LINE_STRING_LITERAL_TAG
{
    EDM_SRID_LITERAL srid;
    EDM_LINE_STRING_LITERAL lineStringLiteral;
} EDM_FULL_LINE_STRING_LITERAL;

/*multiLineStringLiteral     = "MultiLineString(" [ lineStringData *( COMMA lineStringData ) ] CLOSE*/
typedef struct EDM_MULTI_LINE_STRING_LITERAL_TAG
{
    size_t nLineStringDatas;
    EDM_LINE_STRING_DATA lineStringData;
}EDM_MULTI_LINE_STRING_LITERAL;

/*fullMultiLineStringLiteral = sridLiteral multiLineStringLiteral*/
typedef struct EDM_FULL_MULTI_LINE_STRING_LITERAL_TAG
{
    EDM_SRID_LITERAL srid;
    EDM_MULTI_LINE_STRING_LITERAL multiLineStringLiteral;
}EDM_FULL_MULTI_LINE_STRING_LITERAL;



/*forward defines*/
struct EDM_GEO_LITERAL_TAG;
typedef struct EDM_GEO_LITERAL_TAG EDM_GEO_LITERAL;

/*collectionLiteral     = "Collection(" geoLiteral *( COMMA geoLiteral ) CLOSE*/
typedef struct EDM_COLLECTION_LITERAL_TAG
{
    size_t nGeoLiterals;
    EDM_GEO_LITERAL* geoLiterals;
} EDM_COLLECTION_LITERAL;

/*geoLiteral            = collectionLiteral
/ lineStringLiteral
/ multiPointLiteral
/ multiLineStringLiteral
/ multiPolygonLiteral
/ pointLiteral
/ polygonLiteral
*/
typedef enum EDM_GEO_LITERAL_TYPE_TAG
{
    EDM_COLLECTION_LITERAL_TYPE,
    EDM_LINE_STRING_LITERAL_TYPE,
    EDM_MULTI_POINT_LITERAL_TYPE,
    EDM_MULTI_LINE_STRING_LITERAL_TYPE,
    EDM_MULTI_POLIGON_LITERAL_TYPE,
    EDM_POINT_LITERAL_TYPE,
    EDM_POLYGON_LITERAL_TYPE
}EDM_GEO_LITERAL_TYPE;

struct EDM_GEO_LITERAL_TAG
{
    EDM_GEO_LITERAL_TYPE literalType;
    union
    {
        EDM_COLLECTION_LITERAL collectionLiteral;
        EDM_LINE_STRING_LITERAL lineStringLiteral;
        EDM_MULTI_POINT_LITERAL multiPointLiteral;
        EDM_MULTI_LINE_STRING_LITERAL multiLineStringLiteral;
        EDM_MULTI_POLYGON_LITERAL multiPolygonLiteral;
        EDM_POINT_LITERAL pointLiteral;
        EDM_POLYGON_LITERAL polygonLiteral;
    } u;
};

/*fullCollectionLiteral = sridLiteral collectionLiteral*/
typedef struct EDM_FULL_COLLECTION_LITERAL_TAG
{
    EDM_SRID_LITERAL srid;
    EDM_COLLECTION_LITERAL collectionLiteral;
}EDM_FULL_COLLECTION_LITERAL;

/*now geography stuff*/

/*geographyCollection   = geographyPrefix SQUOTE fullCollectionLiteral SQUOTE*/
typedef struct EDM_GEOGRAPHY_COLLECTION_TAG
{
    EDM_FULL_COLLECTION_LITERAL fullCollectionLiteral;
}EDM_GEOGRAPHY_COLLECTION;

/*geographyLineString   = geographyPrefix SQUOTE fullLineStringLiteral SQUOTE*/
typedef struct EDM_GEOGRAPHY_LINE_STRING_TAG
{
    EDM_FULL_LINE_STRING_LITERAL fullLineStringLiteral;
}EDM_GEOGRAPHY_LINE_STRING;

/*geographyMultiLineString   = geographyPrefix SQUOTE fullMultiLineStringLiteral SQUOTE*/
typedef struct EDM_GEOGRAPHY_MULTI_LINE_STRING_TAG
{
    EDM_FULL_MULTI_LINE_STRING_LITERAL fullMultiLineStringLiteral;
}EDM_GEOGRAPHY_MULTI_LINE_STRING;

/*geographyMultiPoint   = geographyPrefix SQUOTE fullMultiPointLiteral SQUOTE*/
typedef struct EDM_GEOGRAPHY_MULTI_POINT_TAG
{
    EDM_FULL_MULTI_POINT_LITERAL fullMultiPointLiteral;
}EDM_GEOGRAPHY_MULTI_POINT;

/*geographyMultiPolygon   = geographyPrefix SQUOTE fullMultiPolygonLiteral SQUOTE*/
typedef struct EDM_GEOGRAPHY_MULTI_POLYGON_TAG
{
    EDM_FULL_MULTI_POLYGON_LITERAL fullMultiPolygonLiteral;
}EDM_GEOGRAPHY_MULTI_POLYGON;

/*geographyPolygon   = geographyPrefix SQUOTE fullPolygonLiteral SQUOTE*/
typedef struct EDM_GEOGRAPHY_POLYGON_TAG
{
    EDM_FULL_POLYGON_LITERAL fullPolygonLiteral;
}EDM_GEOGRAPHY_POLYGON;

/*geometryCollection      = geometryPrefix SQUOTE fullCollectionLiteral      SQUOTE*/
/*forward defines*/


/*geometryPolygon         = geometryPrefix SQUOTE fullPolygonLiteral         SQUOTE*/
typedef struct EDM_GEOMETRY_POLYGON_TAG
{
    EDM_FULL_POLYGON_LITERAL fullPolygonLiteral;
}EDM_GEOMETRY_POLYGON;

/*geometryPoint           = geometryPrefix SQUOTE fullPointLiteral           SQUOTE*/
typedef struct EDM_GEOMETRY_POINT_TAG
{
    EDM_FULL_POINT_LITERAL fullPointLiteral;
}EDM_GEOMETRY_POINT;

/*geometryMultiPolygon    = geometryPrefix SQUOTE fullMultiPolygonLiteral    SQUOTE*/
typedef struct EDM_GEOMETRY_MULTI_POLYGON_TAG
{
    EDM_FULL_MULTI_POLYGON_LITERAL fullMultiPolygonLiteral;
}
EDM_GEOMETRY_MULTI_POLYGON;

/*geometryMultiPoint      = geometryPrefix SQUOTE fullMultiPointLiteral      SQUOTE*/
typedef struct EDM_GEOMETRY_MULTI_POINT_TAG
{
    EDM_FULL_MULTI_POINT_LITERAL fullMultiPointLiteral;
}EDM_GEOMETRY_MULTI_POINT;

/*geometryMultiLineString = geometryPrefix SQUOTE fullMultiLineStringLiteral SQUOTE*/
typedef struct EDM_GEOMETRY_MULTI_LINE_STRING_TAG
{
    EDM_FULL_MULTI_LINE_STRING_LITERAL fullMultiLineStringLiteral;
}EDM_GEOMETRY_MULTI_LINE_STRING;

/*geometryLineString      = geometryPrefix SQUOTE fullLineStringLiteral      SQUOTE*/
typedef struct EDM_GEOMETRY_LINE_STRING_TAG
{
    EDM_FULL_LINE_STRING_LITERAL fullLineStringLiteral;
}EDM_GEOMETRY_LINE_STRING;

/*geometryCollection      = geometryPrefix SQUOTE fullCollectionLiteral      SQUOTE*/
typedef struct EDM_GEOMETRY_COLLECTION_TAG
{
    EDM_FULL_COLLECTION_LITERAL fullCollectionLiteral;
}EDM_GEOMETRY_COLLECTION;

/*holds the name and the value of a COMPLEX_TYPE... field*/
typedef struct COMPLEX_TYPE_FIELD_TAG
{
    const char* fieldName;
    AGENT_DATA_TYPE* value;
}COMPLEX_TYPE_FIELD_TYPE;

/*EDM_COMPLEX_TYPE - this type doesn't exist in EDMX as a primitive type*/
typedef struct EDM_COMPLEX_TYPE_TAG
{
    size_t nMembers;
    COMPLEX_TYPE_FIELD_TYPE *fields;
}EDM_COMPLEX_TYPE;

#define AGENT_DATA_TYPES_RESULT_VALUES \
    AGENT_DATA_TYPES_OK, \
    AGENT_DATA_TYPES_ERROR, \
    AGENT_DATA_TYPES_INVALID_ARG, \
    AGENT_DATA_TYPES_NOT_IMPLEMENTED, \
    AGENT_DATA_TYPES_JSON_ENCODER_ERRROR

MU_DEFINE_ENUM_WITHOUT_INVALID(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_RESULT_VALUES);

#define AGENT_DATA_TYPE_TYPE_VALUES\
    EDM_NO_TYPE,                                                                   \
    EDM_BINARY_TYPE,                                                               \
    EDM_BOOLEAN_TYPE,                                                              \
    EDM_BYTE_TYPE,                                                                 \
    EDM_DATE_TYPE,                                                                 \
    EDM_DATE_TIME_OFFSET_TYPE,                                                     \
    EDM_DECIMAL_TYPE,                                                              \
    EDM_DOUBLE_TYPE,                                                               \
    EDM_DURATION_TYPE,                                                             \
    EDM_GUID_TYPE,                                                                 \
    EDM_INT16_TYPE,                                                                \
    EDM_INT32_TYPE,                                                                \
    EDM_INT64_TYPE,                                                                \
    EDM_SBYTE_TYPE,                                                                \
    EDM_SINGLE_TYPE,                                                               \
    EDM_STREAM, /*not supported, because what is stream?*/                         \
    EDM_STRING_TYPE,                                                               \
    EDM_TIME_OF_DAY_TYPE,                                                          \
    EDM_GEOGRAPHY_TYPE, /*not supported because what is "abstract base type"*/     \
    EDM_GEOGRAPHY_POINT_TYPE,                                                      \
    EDM_GEOGRAPHY_LINE_STRING_TYPE,                                                \
    EDM_GEOGRAPHY_POLYGON_TYPE,                                                    \
    EDM_GEOGRAPHY_MULTI_POINT_TYPE,                                                \
    EDM_GEOGRAPHY_MULTI_LINE_STRING_TYPE,                                          \
    EDM_GEOGRAPHY_MULTI_POLYGON_TYPE,                                              \
    EDM_GEOGRAPHY_COLLECTION_TYPE,                                                 \
    EDM_GEOMETRY_TYPE, /*not supported because what is "abstract base type"*/      \
    EDM_GEOMETRY_POINT_TYPE,                                                       \
    EDM_GEOMETRY_LINE_STRING_TYPE,                                                 \
    EDM_GEOMETRY_POLYGON_TYPE,                                                     \
    EDM_GEOMETRY_MULTI_POINT_TYPE,                                                 \
    EDM_GEOMETRY_MULTI_LINE_STRING_TYPE,                                           \
    EDM_GEOMETRY_MULTI_POLYGON_TYPE,                                               \
    EDM_GEOMETRY_COLLECTION_TYPE,                                                  \
    EDM_COMPLEX_TYPE_TYPE,                                                         \
    EDM_NULL_TYPE,                                                                 \
    EDM_ENTITY_TYPE_TYPE,                                                          \
    EDM_STRING_NO_QUOTES_TYPE                                                      \


MU_DEFINE_ENUM_WITHOUT_INVALID(AGENT_DATA_TYPE_TYPE, AGENT_DATA_TYPE_TYPE_VALUES);

struct AGENT_DATA_TYPE_TAG
{
    AGENT_DATA_TYPE_TYPE type;
    union
    {
        EDM_BINARY edmBinary;
        EDM_BOOLEAN edmBoolean;
        EDM_BYTE edmByte;
        EDM_DATE edmDate;
        EDM_DATE_TIME_OFFSET edmDateTimeOffset;
        EDM_DECIMAL edmDecimal;
        EDM_DOUBLE edmDouble;
        EDM_DURATION edmDuration;
        EDM_GUID edmGuid;
        EDM_INT16 edmInt16;
        EDM_INT32 edmInt32;
        EDM_INT64 edmInt64;
        EDM_SBYTE edmSbyte;
        EDM_SINGLE edmSingle;
        /*EDM_STREAM, not supported, because what is stream?*/
        EDM_STRING edmString;
        EDM_STRING edmStringNoQuotes;
        EDM_TIME_OF_DAY edmTimeOfDay;
        /*EDM_GEOGRAPHY_, not supported because what is "abstract base type"*/
        EDM_GEOGRAPHY_POINT edmGeographyPoint;
        EDM_GEOGRAPHY_LINE_STRING edmGeographyLineString;
        EDM_GEOGRAPHY_POLYGON edmGeographyPolygon;
        EDM_GEOGRAPHY_MULTI_POINT edmGeographyMultiPoint;
        EDM_GEOGRAPHY_MULTI_LINE_STRING edmGeographyMultiLineString;
        EDM_GEOGRAPHY_MULTI_POLYGON edmGeographyMultiPolygon;
        EDM_GEOGRAPHY_COLLECTION edmGeographyCollection;
        /* EDM_GEOMETRY_, not supported because what is "abstract base type"*/
        EDM_GEOMETRY_POINT edmGeometryPoint;
        EDM_GEOMETRY_LINE_STRING edmGeometryLineString;
        EDM_GEOMETRY_POLYGON edmGeometryPolygon;
        EDM_GEOMETRY_MULTI_POINT edmGeometryMultiPoint;
        EDM_GEOMETRY_MULTI_LINE_STRING edmGeoemtryMultiLineString;
        EDM_GEOMETRY_MULTI_POLYGON edmGeometryMultiPolygon;
        EDM_GEOMETRY_COLLECTION edmGeometryCollection;
        EDM_COMPLEX_TYPE edmComplexType;
    } value;
};

#include "umock_c/umock_c_prod.h"

MOCKABLE_FUNCTION(, AGENT_DATA_TYPES_RESULT, AgentDataTypes_ToString, STRING_HANDLE, destination, const AGENT_DATA_TYPE*, value);

/*Create/Destroy work in pairs. For some data type not calling Uncreate might be ok. For some, it will lead to memory leaks*/

/*creates an AGENT_DATA_TYPE containing a EDM_BOOLEAN from a int*/
MOCKABLE_FUNCTION(, AGENT_DATA_TYPES_RESULT, Create_EDM_BOOLEAN_from_int, AGENT_DATA_TYPE*, agentData, int, v);

/*creates an AGENT_DATA_TYPE containing a UINT8*/
MOCKABLE_FUNCTION(, AGENT_DATA_TYPES_RESULT, Create_AGENT_DATA_TYPE_from_UINT8, AGENT_DATA_TYPE*, agentData, uint8_t, v);

/*creates an AGENT_DATA_TYPE containing a EDM_DATE */
MOCKABLE_FUNCTION(, AGENT_DATA_TYPES_RESULT, Create_AGENT_DATA_TYPE_from_date, AGENT_DATA_TYPE*, agentData, int16_t, year, uint8_t, month, uint8_t, day);

/*create an AGENT_DATA_TYPE containing an EDM_DECIMAL from a string representation*/
MOCKABLE_FUNCTION(, AGENT_DATA_TYPES_RESULT, Create_EDM_DECIMAL_from_charz, AGENT_DATA_TYPE*, agentData, const char*, v);

/*create an AGENT_DATA_TYPE containing an EDM_DOUBLE from a double*/
MOCKABLE_FUNCTION(, AGENT_DATA_TYPES_RESULT, Create_AGENT_DATA_TYPE_from_DOUBLE, AGENT_DATA_TYPE*, agentData, double, v);

/*create an AGENT_DATA_TYPE from INT16_T*/
MOCKABLE_FUNCTION(, AGENT_DATA_TYPES_RESULT, Create_AGENT_DATA_TYPE_from_SINT16, AGENT_DATA_TYPE*, agentData, int16_t, v);

/*create an AGENT_DATA_TYPE from INT32_T*/
MOCKABLE_FUNCTION(, AGENT_DATA_TYPES_RESULT, Create_AGENT_DATA_TYPE_from_SINT32, AGENT_DATA_TYPE*, agentData, int32_t, v);

/*create an AGENT_DATA_TYPE from INT64_T*/
MOCKABLE_FUNCTION(, AGENT_DATA_TYPES_RESULT, Create_AGENT_DATA_TYPE_from_SINT64, AGENT_DATA_TYPE*, agentData, int64_t, v);

/*create an AGENT_DATA_TYPE from int8_t*/
MOCKABLE_FUNCTION(, AGENT_DATA_TYPES_RESULT, Create_AGENT_DATA_TYPE_from_SINT8, AGENT_DATA_TYPE*, agentData, int8_t, v);

/*Codes_SRS_AGENT_TYPE_SYSTEM_99_091:[Creates an AGENT_DATA_TYPE containing an Edm.DateTimeOffset from an EDM_DATE_TIME_OFFSET.]*/
MOCKABLE_FUNCTION(, AGENT_DATA_TYPES_RESULT, Create_AGENT_DATA_TYPE_from_EDM_DATE_TIME_OFFSET, AGENT_DATA_TYPE*, agentData, EDM_DATE_TIME_OFFSET, v);

/*creates an AGENT_DATA_TYPE containing a EDM_GUID*/
MOCKABLE_FUNCTION(, AGENT_DATA_TYPES_RESULT, Create_AGENT_DATA_TYPE_from_EDM_GUID, AGENT_DATA_TYPE*, agentData, EDM_GUID, v);

/*creates an AGENT_DATA_TYPE containing a EDM_BINARY*/
MOCKABLE_FUNCTION(, AGENT_DATA_TYPES_RESULT, Create_AGENT_DATA_TYPE_from_EDM_BINARY, AGENT_DATA_TYPE*, agentData, EDM_BINARY, v);

/*create an AGENT_DATA_TYPE from SINGLE*/
MOCKABLE_FUNCTION(, AGENT_DATA_TYPES_RESULT, Create_AGENT_DATA_TYPE_from_FLOAT, AGENT_DATA_TYPE*, agentData, float, v);

/*create an AGENT_DATA_TYPE from ANSI zero terminated string*/
MOCKABLE_FUNCTION(, AGENT_DATA_TYPES_RESULT, Create_AGENT_DATA_TYPE_from_charz, AGENT_DATA_TYPE*, agentData, const char*, v);

/*create an AGENT_DATA_TYPE from ANSI zero terminated string (no quotes)*/
MOCKABLE_FUNCTION(, AGENT_DATA_TYPES_RESULT, Create_AGENT_DATA_TYPE_from_charz_no_quotes, AGENT_DATA_TYPE*, agentData, const char*, v);

/*create an AGENT_DATA_TYPE of type EDM_NULL_TYPE */
MOCKABLE_FUNCTION(, AGENT_DATA_TYPES_RESULT, Create_NULL_AGENT_DATA_TYPE, AGENT_DATA_TYPE*, agentData);

/*create an AGENT_DATA_TYPE that holds a structs from its fields*/
MOCKABLE_FUNCTION(, AGENT_DATA_TYPES_RESULT, Create_AGENT_DATA_TYPE_from_Members, AGENT_DATA_TYPE*,agentData, const char*, typeName, size_t, nMembers, const char* const *, memberNames, const AGENT_DATA_TYPE*, memberValues);

/*create a complex AGENT_DATA_TYPE from pointers to AGENT_DATA_TYPE fields*/
MOCKABLE_FUNCTION(, AGENT_DATA_TYPES_RESULT, Create_AGENT_DATA_TYPE_from_MemberPointers, AGENT_DATA_TYPE*, agentData, const char*, typeName, size_t, nMembers, const char* const *, memberNames, const AGENT_DATA_TYPE** ,memberPointerValues);

/*creates a copy of the AGENT_DATA_TYPE*/
MOCKABLE_FUNCTION(, AGENT_DATA_TYPES_RESULT, Create_AGENT_DATA_TYPE_from_AGENT_DATA_TYPE, AGENT_DATA_TYPE*, dest, const AGENT_DATA_TYPE*, src);

MOCKABLE_FUNCTION(, void, Destroy_AGENT_DATA_TYPE, AGENT_DATA_TYPE*, agentData);

MOCKABLE_FUNCTION(, AGENT_DATA_TYPES_RESULT, CreateAgentDataType_From_String, const char*, source, AGENT_DATA_TYPE_TYPE, type, AGENT_DATA_TYPE*, agentData);

MOCKABLE_FUNCTION(, COMPLEX_TYPE_FIELD_TYPE*, AgentDataType_GetComplexTypeField, AGENT_DATA_TYPE*, agentData, size_t, index);

#ifdef __cplusplus
}
#endif

#endif
