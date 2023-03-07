// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef MACRO_UTILS_H
#define MACRO_UTILS_H

#include "azure_macro_utils/macro_utils_generated.h"

#ifdef __cplusplus
#include <cstring>
#include <cstddef>
extern "C" {
#else
#include <string.h>
#include <stddef.h>
#endif

#if (defined OPTIMIZE_RETURN_CODES)
    #define MU_FAILURE 1
#else
    #define MU_FAILURE __LINE__
#endif

/*"pointer or NULL" macro - because when printf-ing arguments NULL is not valid for %s (section 7.1.4 of C11 standard) */
#define MU_P_OR_NULL(p) (((p)!=NULL)?(p):"NULL")
#define MU_WP_OR_NULL(p) (((p)!=NULL)?(p):L"NULL")

#define MU_TOSTRING_(x) #x
#define MU_TOSTRING(x) MU_TOSTRING_(x)

#define MU_TRIGGER_PARENTHESIS(...) ,

#define MU_LPAREN (

#define MU_C2_(x,y) x##y

#define MU_C2(x,y) MU_C2_(x,y)

#define MU_C3(x,y,z) MU_C2(x, MU_C2(y,z))

#define MU_C4(x,y,z, u) MU_C2(MU_C2(x,y), MU_C2(z,u))

#define MU_C5(x,y,z,u, v) MU_C2(MU_C4(x,y, z, u), v)

#define MU_C1_(x) x

#define MU_C1(x) MU_C1_(x)

#define MU_C2STRING(x,y) x y

#define MU_C3STRING(x,y,z) x y z

#define MU_C4STRING(x,y,z,u) x y z u

#define MU_C5STRING(x,y,z,u,v) x y z u v

/* we need some sort of macro that does:
MU_IF(0, "true", "false") => "false"
MU_IF(1, "true", "false") => "true"
MU_IF(X, "true", "false") => "true"
*/

#define MU_INTERNALIF(x) MU_INTERNALIF##x
#define MU_INTERNALIF0

#define MU_ISZERO(x) MU_COUNT_ARG(MU_INTERNALIF(x))

#define MU_IF(condition, trueBranch, falseBranch) MU_C2(MU_IF,MU_ISZERO(condition))(trueBranch, falseBranch)
#define MU_IF0(trueBranch, falseBranch) falseBranch
#define MU_IF1(trueBranch, falseBranch) trueBranch


/*the following macro want to eat empty arguments from a list */
/*examples:                                                   */
/*MU_EAT_EMPTY_ARGS(, , X) expands to X                       */

#define MU_EXPAND_TO_NOTHING(arg)
#define MU_EAT_EMPTY_ARG(arg_count, arg) MU_IF(MU_ISEMPTY(arg),,arg) MU_IF(MU_ISEMPTY(arg),MU_EXPAND_TO_NOTHING,MU_IFCOMMALOGIC)(MU_DEC(arg_count))
#define MU_EAT_EMPTY_ARGS(...) MU_FOR_EACH_1_COUNTED(MU_EAT_EMPTY_ARG, __VA_ARGS__)

#define MU_DEFINE_ENUMERATION_CONSTANT(x) x,
/*MU_DEFINE_ENUM_WITHOUT_INVALID goes to header*/
#define MU_DEFINE_ENUM_WITHOUT_INVALID(enumName, ...) typedef enum MU_C2(enumName, _TAG) { MU_FOR_EACH_1(MU_DEFINE_ENUMERATION_CONSTANT, __VA_ARGS__)} enumName; \
    extern const char* MU_C2(enumName,Strings)(enumName value); \
    extern int MU_C2(enumName, _FromString)(const char* enumAsString, enumName* destination);

// this macro is a wrapper on top of MU_DEFINE_ENUM_WITHOUT_INVALID, adding an _INVALID value as the first enum value in the enum
#define MU_DEFINE_ENUM(enumName, ...) \
    MU_DEFINE_ENUM_WITHOUT_INVALID(enumName, MU_C2(enumName, _INVALID), __VA_ARGS__)

#define MU_DEFINE_ENUMERATION_CONSTANT_AS_WIDESTRING(x) MU_C2(L, MU_TOSTRING(x)) , 
#define MU_DEFINE_ENUMERATION_CONSTANT_AS_STRING(x) MU_TOSTRING(x) , 
/*MU_DEFINE_ENUM_STRINGS_WITHOUT_INVALID goes to .c*/
#define MU_DEFINE_ENUM_STRINGS_WITHOUT_INVALID(enumName, ...) const char* MU_C2(enumName, StringStorage)[MU_COUNT_ARG(__VA_ARGS__)] = {MU_FOR_EACH_1(MU_DEFINE_ENUMERATION_CONSTANT_AS_STRING, __VA_ARGS__)}; \
const char* MU_C2(enumName,Strings)(enumName value)                \
{                                                                  \
    if((int)value<0 || (int)value>=MU_COUNT_ARG(__VA_ARGS__))      \
    {                                                              \
        /*this is an error case*/                                  \
        return "NULL";                                             \
    }                                                              \
    else                                                           \
    {                                                              \
        return MU_C2(enumName, StringStorage)[value];              \
    }                                                              \
}                                                                  \
int MU_C2(enumName, _FromString)(const char* enumAsString, enumName* destination)  \
{                                                                               \
    if(                                                                         \
        (enumAsString==NULL) || (destination==NULL)                             \
    )                                                                           \
    {                                                                           \
        return MU_FAILURE;                                                      \
    }                                                                           \
    else                                                                        \
    {                                                                           \
        size_t i;                                                               \
        for(i=0;i<MU_COUNT_ARG(__VA_ARGS__);i++)                                \
        {                                                                       \
            if(strcmp(enumAsString, MU_C2(enumName, StringStorage)[i])==0)      \
            {                                                                   \
                *destination = (enumName)i;                                     \
                return 0;                                                       \
            }                                                                   \
        }                                                                       \
        return MU_FAILURE;                                                      \
    }                                                                           \
}                                                                               \

// this macro is a wrapper on top of MU_DEFINE_ENUM_STRINGS_WITHOUT_INVALID, adding an _INVALID value as the first enum value in the enum
#define MU_DEFINE_ENUM_STRINGS(enumName, ...) \
    MU_DEFINE_ENUM_STRINGS_WITHOUT_INVALID(enumName, MU_C2(enumName, _INVALID), __VA_ARGS__)

#define MU_DEFINE_LOCAL_ENUM_WITHOUT_INVALID(enumName, ...) typedef enum MU_C2(enumName, _TAG) { MU_FOR_EACH_1(MU_DEFINE_ENUMERATION_CONSTANT, __VA_ARGS__)} enumName; \
static const char* MU_C2(enumName, StringStorage)[MU_COUNT_ARG(__VA_ARGS__)] = {MU_FOR_EACH_1(MU_DEFINE_ENUMERATION_CONSTANT_AS_STRING, __VA_ARGS__)}; \
static const char* MU_C2(enumName,Strings)(enumName value)         \
{                                                                  \
    if((int)value<0 || (int)value>=MU_COUNT_ARG(__VA_ARGS__))      \
    {                                                              \
        /*this is an error case*/                                  \
        return "NULL";                                             \
    }                                                              \
    else                                                           \
    {                                                              \
        return MU_C2(enumName, StringStorage)[value];              \
    }                                                              \
}

// this macro is a wrapper on top of MU_DEFINE_LOCAL_ENUM_WITHOUT_INVALID, adding an _INVALID value as the first enum value in the enum
#define MU_DEFINE_LOCAL_ENUM(enumName, ...) \
    MU_DEFINE_LOCAL_ENUM_WITHOUT_INVALID(enumName, MU_C2(enumName, _INVALID), __VA_ARGS__)

// this macro returns the number of enum values (taking into account that an invalid value is generated)
#define MU_ENUM_VALUE_COUNT(...) (MU_COUNT_ARG(__VA_ARGS__) + 1)

// this macro returns the number of enum 2 values (taking into account that an invalid value is generated)
#define MU_ENUM_2_VALUE_COUNT(...) ((MU_COUNT_ARG(__VA_ARGS__) / 2) + 1)

#define MU_ENUM_TO_STRING(enumName, enumValue) MU_C2(enumName, Strings)(enumValue)
#define MU_STRING_TO_ENUM(stringValue, enumName, addressOfEnumVariable) MU_C2(enumName, _FromString)(stringValue, addressOfEnumVariable)

#define MU_EMPTY()
#define MACRO_UTILS_DELAY(id) id MU_EMPTY MU_LPAREN )

#define MU_DEFINE_ENUMERATION_CONSTANT_2(enumerationConstant, constantExpression) enumerationConstant = constantExpression,

#define MU_DECLARE_ENUM_STRINGS_2(enumIdentifier, ...) extern const char* MU_C3(MU_, enumIdentifier,_ToString)(enumIdentifier enumerationConstant);

#define MU_DEFINE_ENUM_2_WITHOUT_INVALID(enumIdentifier, ... ) typedef enum MU_C2(enumIdentifier, _TAG) {MU_FOR_EACH_2(MU_DEFINE_ENUMERATION_CONSTANT_2, __VA_ARGS__)} enumIdentifier; \
    MU_DECLARE_ENUM_STRINGS_2(enumIdentifier, __VA_ARGS__)

// this macro is a wrapper on top of MU_DEFINE_ENUM, adding an _INVALID value as the first enum value in the enum
#define MU_DEFINE_ENUM_2(enumName, ...) \
    MU_DEFINE_ENUM_2_WITHOUT_INVALID(enumName, MU_C2(enumName, _INVALID), (int)0xDDDDDDDD, __VA_ARGS__)

typedef struct ENUM_VALUE_AND_STRING_TAG
{
    int value;
    const char* valueAsString;
}ENUM_VALUE_AND_STRING;

#define MU_DEFINE_ENUM_VALUE_AND_STRING(enumerationConstant, constantExpression) {enumerationConstant, MU_TOSTRING(enumerationConstant)},
#define MU_DEFINE_ENUM_STRINGS_2(enumIdentifier, ... ) static const ENUM_VALUE_AND_STRING MU_C2(enumIdentifier, _ValuesAndStrings)[MU_DIV2(MU_COUNT_ARG(__VA_ARGS__))] ={MU_FOR_EACH_2(MU_DEFINE_ENUM_VALUE_AND_STRING, __VA_ARGS__)}; \
const char* MU_C3(MU_, enumIdentifier,_ToString)(enumIdentifier value)                                                                                                                                                       \
{                                                                                                                                                                                                                    \
    for(size_t i=0;i<sizeof(MU_C2(enumIdentifier, _ValuesAndStrings))/sizeof(MU_C2(enumIdentifier, _ValuesAndStrings)[0]);i++)                                                                                             \
    {                                                                                                                                                                                                                \
        if(MU_C2(enumIdentifier, _ValuesAndStrings)[i].value == (int)value)                                                                                                                                             \
        {                                                                                                                                                                                                            \
            return MU_C2(enumIdentifier, _ValuesAndStrings)[i].valueAsString;                                                                                                                                           \
        }                                                                                                                                                                                                            \
    }                                                                                                                                                                                                                \
    return "NULL";                                                                                                                                                                                                   \
}                                                                                                                                                                                                                    \

#define MU_ENUM_TO_STRING_2(enumIdentifier, value) MU_C3(MU_, enumIdentifier,_ToString)(value)

/*PRI_MU_ENUM and MU_ENUM_VALUE/MU_ENUM_VALUE_2 work together as printf format specifier/argument. e.g: printf("enumValue was=%" PRI_MU_ENUM "\n", MU_ENUM_TO_STRING(enumIdentifier, enumValue));*/
#define PRI_MU_ENUM "s%s (%d)"

#define MU_ENUM_VALUE(enumIdentifier, value) "", MU_ENUM_TO_STRING(enumIdentifier, (value)), (int)(value)

#define MU_ENUM_VALUE_2(enumIdentifier, value) "", MU_ENUM_TO_STRING_2(enumIdentifier, (value)), (int)(value)

#define MU_DEFINE_STRUCT_FIELD(fieldType, fieldName) fieldType fieldName;

/*MU_DEFINE_STRUCT allows creating a struct typedef based on a list of fields*/
#define MU_DEFINE_STRUCT(structName, ...) typedef struct MU_C2(structName, _TAG) { MU_FOR_EACH_2(MU_DEFINE_STRUCT_FIELD, __VA_ARGS__)} structName;

// this macro allows counting of elements in an array
#define MU_COUNT_ARRAY_ITEMS(A) (sizeof(A)/sizeof((A)[0]))

#ifdef __cplusplus
}
#endif

#endif /*MACRO_UTILS_H*/
