// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

/** @file   serializer.h
*    @brief    The IoT Hub Serializer APIs allows developers to define models for
*            their devices
*
*    @details    The IoT Hub Serializer APIs allows developers to quickly and easily define
*                models for their devices directly as code, while supporting the required
*                features for modeling devices (including multiple models and multiple
*                devices within the same application). For example:
*
*        <pre>
*       BEGIN_NAMESPACE(Contoso);
*
*           DECLARE_STRUCT(SystemProperties,
*               ascii_char_ptr, DeviceID,
*               _Bool, Enabled
*           );
*
*           DECLARE_MODEL(VendingMachine,
*
*               WITH_DATA(int, SensorValue),
*
*               WITH_DATA(ascii_char_ptr, ObjectName),
*               WITH_DATA(ascii_char_ptr, ObjectType),
*               WITH_DATA(ascii_char_ptr, Version),
*               WITH_DATA(SystemProperties, SystemProperties),
*               WITH_DATA(ascii_char_ptr_no_quotes, Commands),
*
*               WITH_ACTION(SetItemPrice, ascii_char_ptr, itemId, ascii_char_ptr, price)
*           );
*
*       END_NAMESPACE(Contoso);
*       </pre>
*/

#ifndef SERIALIZER_H
#define SERIALIZER_H

#ifdef __cplusplus
#include <cstdlib>
#include <cstdarg>

#else
#include <stdlib.h>
#include <stdarg.h>
#endif

#include "azure_c_shared_utility/gballoc.h"
#include "azure_macro_utils/macro_utils.h"
#include "iotdevice.h"
#include "azure_c_shared_utility/crt_abstractions.h"
#include "azure_c_shared_utility/xlogging.h"
#include "methodreturn.h"
#include "schemalib.h"
#include "codefirst.h"
#include "agenttypesystem.h"
#include "schema.h"



#ifdef __cplusplus
extern "C"
{
#endif

    /* IOT Agent Macros */

    /**
     * @def BEGIN_NAMESPACE(schemaNamespace)
     * This macro marks the start of a section that declares IOT model
     * elements (like complex types, etc.). Declarations are typically
     * placed in header files, so that they can be shared between
     * translation units.
     */
     /* Codes_SRS_SERIALIZER_99_001:[For each completed schema declaration block, a unique storage container for schema metadata shall be available in the translation unit at runtime.] */
#define BEGIN_NAMESPACE(schemaNamespace) \
    REFLECTED_END_OF_LIST

/**
* @def END_NAMESPACE(schemaNamespace)
* This macro marks the end of a section that declares IOT model
* elements.
*/
#define END_NAMESPACE(schemaNamespace) \
    REFLECTED_LIST_HEAD(schemaNamespace)

#define GLOBAL_INITIALIZE_STRUCT_FIELD(structType, destination, type, name) GlobalInitialize_##type((char*)destination+offsetof(structType, name));
#define GLOBAL_DEINITIALIZE_STRUCT_FIELD(structType, destination, type, name) GlobalDeinitialize_##type((char*)destination+offsetof(structType, name));
/**
* @def DECLARE_STRUCT(name, ...)
* This macro allows the definition of a struct type that can then be used as
* part of a model definition.
*
* @param name                      Name of the struct
* @param element1, element2...     Specifies a list of struct members
*/
/* Codes_SRS_SERIALIZER_99_080:[ The DECLARE_STRUCT declaration shall insert metadata describing a complex data type.] */
#define DECLARE_STRUCT(name, ...) \
    /* Codes_SRS_SERIALIZER_99_096:[ DECLARE_STRUCT shall declare a matching C struct data type named name, which can be referenced from any code that can access the declaration.] */ \
    typedef struct name##_TAG { \
        MU_FOR_EACH_2(INSERT_FIELD_INTO_STRUCT, __VA_ARGS__) \
    } name; \
    /* Codes_SRS_SERIALIZER_99_081:[ DECLARE_STRUCT's name argument shall uniquely identify the struct within the schema.] */ \
    REFLECTED_STRUCT(name) \
    /* Codes_SRS_SERIALIZER_99_082:[ DECLARE_STRUCT's field<n>Name argument shall uniquely name a field within the struct.] */ \
    MU_FOR_EACH_2_KEEP_1(REFLECTED_FIELD, name, __VA_ARGS__) \
    TO_AGENT_DATA_TYPE(name, __VA_ARGS__) \
    /*Codes_SRS_SERIALIZER_99_042:[ The parameter types are either predefined parameter types (specs SRS_SERIALIZER_99_004-SRS_SERIALIZER_99_014) or a type introduced by DECLARE_STRUCT.]*/ \
    static AGENT_DATA_TYPES_RESULT FromAGENT_DATA_TYPE_##name(const AGENT_DATA_TYPE* source, name* destination) \
    { \
        AGENT_DATA_TYPES_RESULT result; \
        if(source->type != EDM_COMPLEX_TYPE_TYPE) \
        { \
            result = AGENT_DATA_TYPES_INVALID_ARG; \
        } \
        else if(MU_DIV2(MU_COUNT_ARG(__VA_ARGS__)) != source->value.edmComplexType.nMembers) \
        { \
            /*too many or too few fields*/ \
            result = AGENT_DATA_TYPES_INVALID_ARG; \
        } \
        else \
        { \
            result = AGENT_DATA_TYPES_OK; \
            MU_FOR_EACH_2(BUILD_DESTINATION_FIELD, __VA_ARGS__); \
        } \
        return result; \
    } \
    static void MU_C2(destroyLocalParameter, name)(name * value) \
    { \
        MU_FOR_EACH_2_KEEP_1(UNBUILD_DESTINATION_FIELD, value, __VA_ARGS__); \
    } \
    static void MU_C2(GlobalInitialize_, name)(void* destination) \
    { \
        MU_FOR_EACH_2_KEEP_2(GLOBAL_INITIALIZE_STRUCT_FIELD, name, destination, __VA_ARGS__); \
    } \
    static void MU_C2(GlobalDeinitialize_, name)(void* destination) \
    { \
        MU_FOR_EACH_2_KEEP_2(GLOBAL_DEINITIALIZE_STRUCT_FIELD, name, destination, __VA_ARGS__); \
    } \


/**
 * @def     DECLARE_MODEL(name, ...)
 * This macro allows declaring a model that can be later used to instantiate
 * a device.
 *
 * @param   name                        Specifies the model name
 * @param   element1, element2...       Specifies a model element which can be
 *                                         a property or an action.
 *                                             - A property is described in a
 *                                               model by using the WITH_DATA
 *                                             - An action is described in a
 *                                               model by using the ::WITH_ACTION
 *                                               macro.
 *
 */
 /* WITH_DATA's name argument shall be one of the following data types: */
 /* Codes_SRS_SERIALIZER_99_133:[a model type introduced previously by DECLARE_MODEL] */

#define CREATE_DESIRED_PROPERTY_CALLBACK_MODEL_ACTION(...)
#define CREATE_DESIRED_PROPERTY_CALLBACK_MODEL_METHOD(...)
#define CREATE_DESIRED_PROPERTY_CALLBACK_MODEL_DESIRED_PROPERTY(type, name, ...) MU_IF(MU_COUNT_ARG(__VA_ARGS__), void __VA_ARGS__ (void*);, )
#define CREATE_DESIRED_PROPERTY_CALLBACK_MODEL_PROPERTY(...)
#define CREATE_DESIRED_PROPERTY_CALLBACK_MODEL_REPORTED_PROPERTY(...)

#define CREATE_DESIRED_PROPERTY_CALLBACK(...) CREATE_DESIRED_PROPERTY_CALLBACK_##__VA_ARGS__

#define SERIALIZER_REGISTER_NAMESPACE(NAMESPACE) CodeFirst_RegisterSchema(#NAMESPACE, & ALL_REFLECTED(NAMESPACE))

#define DECLARE_MODEL(name, ...)                                                             \
    REFLECTED_MODEL(name)                                                                    \
    MU_FOR_EACH_1(CREATE_DESIRED_PROPERTY_CALLBACK, __VA_ARGS__)                                \
    typedef struct name { int :1; MU_FOR_EACH_1(BUILD_MODEL_STRUCT, __VA_ARGS__) } name;        \
    MU_FOR_EACH_1_KEEP_1(CREATE_MODEL_ELEMENT, name, __VA_ARGS__)                               \
    TO_AGENT_DATA_TYPE(name, DROP_FIRST_COMMA_FROM_ARGS(EXPAND_MODEL_ARGS(__VA_ARGS__)))     \
    int FromAGENT_DATA_TYPE_##name(const AGENT_DATA_TYPE* source, void* destination)         \
    {                                                                                        \
        (void)source;                                                                        \
        (void)destination;                                                                   \
        LogError("SHOULD NOT GET CALLED... EVER");                                           \
        return 0;                                                                            \
    }                                                                                        \
    static void MU_C2(GlobalInitialize_, name)(void* destination)                               \
    {                                                                                        \
        (void)destination;                                                                   \
        MU_FOR_EACH_1_KEEP_1(CREATE_MODEL_ELEMENT_GLOBAL_INITIALIZE, name, __VA_ARGS__)         \
    }                                                                                        \
    static void MU_C2(GlobalDeinitialize_, name)(void* destination)                             \
    {                                                                                        \
        (void)destination;                                                                   \
        MU_FOR_EACH_1_KEEP_1(CREATE_MODEL_ELEMENT_GLOBAL_DEINITIALIZE, name, __VA_ARGS__)       \
    }                                                                                        \



/**
 * @def   WITH_DATA(type, name)
 * The ::WITH_DATA macro allows declaring a model property in a model. A
 * property can be published by using the ::SERIALIZE macro.
 *
 * @param   type    Specifies the property type. Can be any of the following
 *                  types:
 *                   - int
 *                   - double
 *                   - float
 *                   - long
 *                   - int8_t
 *                   - uint8_t
 *                   - int16_t
 *                   - int32_t
 *                   - int64_t
 *                   - bool
 *                   - ascii_char_ptr
 *                   - EDM_DATE_TIME_OFFSET
 *                   - EDM_GUID
 *                   - EDM_BINARY
 *                   - Any struct type previously introduced by another ::DECLARE_STRUCT.
 *
 * @param   name    Specifies the property name
 */
#define WITH_DATA(type, name) MODEL_PROPERTY(type, name)


#define WITH_REPORTED_PROPERTY(type, name) MODEL_REPORTED_PROPERTY(type, name)

#define WITH_DESIRED_PROPERTY(type, name, ...) MODEL_DESIRED_PROPERTY(type, name, __VA_ARGS__)

/**
 * @def   WITH_ACTION(name, ...)
 * The ::WITH_ACTION macro allows declaring a model action.
 *
 * @param   name                    Specifies the action name.
 * @param   argXtype, argXName...   Defines the type and name for the X<sup>th</sup>
 *                                  argument of the action. The type can be any of
 *                                  the primitive types or a struct type.
 */
/*Codes_SRS_SERIALIZER_99_039:[WITH_ACTION shall declare an action of the current data provider called as the first macro parameter(name) and having the first parameter called parameter1Name of type parameter1Type, the second parameter named parameter2Name having the type parameter2Type and so on.]*/
#define WITH_ACTION(name, ...)  MODEL_ACTION(name, __VA_ARGS__)


/**
* @def   WITH_METHOD(name, ...)
* The ::WITH_METHOD macro allows declaring a model method.
*
* @param   name                    Specifies the method name.
* @param   argXtype, argXName...   Defines the type and name for the X<sup>th</sup>
*                                  argument of the method. The type can be any of
*                                  the primitive types or a struct type.
*/
/*Codes_SRS_SERIALIZER_H_02_029: [ WITH_METHOD shall declare a function with the signature 'METHODRETURN_HANDLE name(param1Type param1Name, ...)', which the developer can define to receive corresponding commands from the IoT service. ]*/
#define WITH_METHOD(name, ...)  MODEL_METHOD(name, __VA_ARGS__)


/**
 * @def   GET_MODEL_HANDLE(schemaNamespace, modelName)
 * The ::GET_MODEL_HANDLE macro returns a model handle that can be used in
 * subsequent operations like generating the CSDL schema for the model,
 * uploading the schema, creating a device, etc.
 *
 * @param   schemaNamespace The namespace to which the model belongs.
 * @param   modelName       The name of the model.
 */
/* Codes_SRS_SERIALIZER_99_110:[ The GET_MODEL_HANDLE function macro shall first register the schema by calling CodeFirst_RegisterSchema, passing schemaNamespace and a pointer to the metadata generated in the schema declaration block.] */
/* Codes_SRS_SERIALIZER_99_094:[ GET_MODEL_HANDLE shall then call Schema_GetModelByName, passing the schemaHandle obtained from CodeFirst_RegisterSchema and modelName arguments, to retrieve the SCHEMA_MODEL_TYPE_HANDLE corresponding to the modelName argument.] */
/* Codes_SRS_SERIALIZER_99_112:[ GET_MODEL_HANDLE will return the handle for the named model.] */
#define GET_MODEL_HANDLE(schemaNamespace, modelName) \
    Schema_GetModelByName(CodeFirst_RegisterSchema(MU_TOSTRING(schemaNamespace), &ALL_REFLECTED(schemaNamespace)), #modelName)

/* Codes_SRS_SERIALIZER_01_002: [If the argument serializerIncludePropertyPath is specified, its value shall be passed to CodeFirst_Create.] */
#define CREATE_DEVICE_WITH_INCLUDE_PROPERTY_PATH(schemaNamespace, modelName, serializerIncludePropertyPath) \
    (modelName*)CodeFirst_CreateDevice(GET_MODEL_HANDLE(schemaNamespace, modelName), &ALL_REFLECTED(schemaNamespace), sizeof(modelName), serializerIncludePropertyPath)

/* Codes_SRS_SERIALIZER_01_003: [If the argument serializerIncludePropertyPath is not specified, CREATE_MODEL_INSTANCE shall pass false to CodeFirst_Create.] */
#define CREATE_DEVICE_WITHOUT_INCLUDE_PROPERTY_PATH(schemaNamespace, modelName) \
    (modelName*)CodeFirst_CreateDevice(GET_MODEL_HANDLE(schemaNamespace, modelName), &ALL_REFLECTED(schemaNamespace), sizeof(modelName), false)

/* Codes_SRS_SERIALIZER_99_104:[ CREATE_MODEL_INSTANCE shall call GET_MODEL_HANDLE, passing schemaNamespace and modelName, to get a model handle representing the model defined in the corresponding schema declaration block.] */
/* Codes_SRS_SERIALIZER_99_106:[ CREATE_MODEL_INSTANCE shall call CodeFirst_CreateDevice, passing the model handle (SCHEMA_MODEL_TYPE_HANDLE]*/
/* Codes_SRS_SERIALIZER_99_107:[ If CodeFirst_CreateDevice fails, CREATE_MODEL_INSTANCE shall return NULL.] */
/* Codes_SRS_SERIALIZER_99_108:[ If CodeFirst_CreateDevice succeeds, CREATE_MODEL_INSTANCE shall return a pointer to an instance of the C struct representing the model for the device.] */
#define CREATE_MODEL_INSTANCE(schemaNamespace, ...) \
    MU_IF(MU_DIV2(MU_COUNT_ARG(__VA_ARGS__)), CREATE_DEVICE_WITH_INCLUDE_PROPERTY_PATH, CREATE_DEVICE_WITHOUT_INCLUDE_PROPERTY_PATH) (schemaNamespace, __VA_ARGS__)

/* Codes_SRS_SERIALIZER_99_109:[ DESTROY_MODEL_INSTANCE shall call CodeFirst_DestroyDevice, passing the pointer returned from CREATE_MODEL_INSTANCE, to release all resources associated with the device.] */
#define DESTROY_MODEL_INSTANCE(deviceData) \
    CodeFirst_DestroyDevice(deviceData)

/**
 * @def      SERIALIZE(destination, destinationSize,...)
 * This macro produces JSON serialized representation of the properties.
 *
 * @param   destination                  Pointer to an @c unsigned @c char* that
 *                                       will receive the serialized data.
 * @param   destinationSize              Pointer to a @c size_t that gets
 *                                       written with the size in bytes of the
 *                                       serialized data
 * @param    property1, property2...     A list of property values to send. The
 *                                       order in which the properties appear in
 *                                       the list does not matter, all values
 *                                       will be sent together.
 *
 */
/*Codes_SRS_SERIALIZER_99_113:[ SERIALIZE shall call CodeFirst_SendAsync, passing a destination, destinationSize, the number of properties to publish, and pointers to the values for each property.] */
/*Codes_SRS_SERIALIZER_99_117:[ If CodeFirst_SendAsync succeeds, SEND will return IOT_AGENT_OK.] */
/*Codes_SRS_SERIALIZER_99_114:[ If CodeFirst_SendAsync fails, SEND shall return IOT_AGENT_SERIALIZE_FAILED.] */
#define SERIALIZE(destination, destinationSize,...) CodeFirst_SendAsync(destination, destinationSize, MU_COUNT_ARG(__VA_ARGS__) MU_FOR_EACH_1(ADDRESS_MACRO, __VA_ARGS__))

#define SERIALIZE_REPORTED_PROPERTIES(destination, destinationSize,...) CodeFirst_SendAsyncReported(destination, destinationSize, MU_COUNT_ARG(__VA_ARGS__) MU_FOR_EACH_1(ADDRESS_MACRO, __VA_ARGS__))


#define IDENTITY_MACRO(x) ,x
#define SERIALIZE_REPORTED_PROPERTIES_FROM_POINTERS(destination, destinationSize, ...) CodeFirst_SendAsyncReported(destination, destinationSize, MU_COUNT_ARG(__VA_ARGS__) MU_FOR_EACH_1(IDENTITY_MACRO, __VA_ARGS__))

/**
 * @def   EXECUTE_COMMAND(device, command)
 * Any action that is declared in a model must also have an implementation as
 * a C function.
 *
 * @param   device      Pointer to device data.
 * @param   command     Values that match the arguments declared in the model
 *                      action.
 */
/*Codes_SRS_SERIALIZER_02_018: [EXECUTE_COMMAND macro shall call CodeFirst_ExecuteCommand passing device, commandBuffer and commandBufferSize.]*/
#define EXECUTE_COMMAND(device, command) (CodeFirst_ExecuteCommand(device, command))

/**
* @def   EXECUTE_METHOD(device, methodName, methodPayload)
* Any method that is declared in a model must also have an implementation as
* a C function.
*
* @param   device      Pointer to device data.
* @param   methodName       The method name.
* @param   methodPayload    The method payload.
*/
#define EXECUTE_METHOD(device, methodName, methodPayload) CodeFirst_ExecuteMethod(device, methodName, methodPayload)

/**
* @def   INGEST_DESIRED_PROPERTIES(device, desiredProperties)
*
* @param   device                return of CodeFirst_CreateDevice.
* @param   desiredProperties     a null terminated string containing in JSON format the desired properties
*/
#define INGEST_DESIRED_PROPERTIES(device, jsonPayload, parseDesiredNode) (CodeFirst_IngestDesiredProperties(device, jsonPayload, parseDesiredNode))

/* Helper macros */

/* These macros remove a useless comma from the beginning of an argument list that looks like:
,x1,y1,x2,y2 */
#ifdef _MSC_VER

#define DROP_FIRST_COMMA(N, x) \
x MU_IFCOMMA_NOFIRST(N)

#define DROP_IF_EMPTY(N, x) \
MU_IF(MU_COUNT_ARG(x),DROP_FIRST_COMMA(N,x),x)

#define DROP_FIRST_COMMA_FROM_ARGS(...) \
MU_FOR_EACH_1_COUNTED(DROP_IF_EMPTY, MU_C1(__VA_ARGS__))

#else

#define DROP_FIRST_COMMA_0(N, x) \
    x MU_IFCOMMA_NOFIRST(N)

#define DROP_FIRST_COMMA_1(N, x) \
    x

#define DROP_FIRST_COMMA(empty, N, x) \
    MU_C2(DROP_FIRST_COMMA_,empty)(N,x)

#define DROP_IF_EMPTY(N, x) \
    DROP_FIRST_COMMA(MU_ISEMPTY(x),N,x)

#define DROP_FIRST_COMMA_FROM_ARGS(...) \
    MU_FOR_EACH_1_COUNTED(DROP_IF_EMPTY, __VA_ARGS__)

#endif

/* These macros expand a sequence of arguments for DECLARE_MODEL that looks like
WITH_DATA(x, y), WITH_DATA(x2, y2) to a list of arguments consumed by the macro that marshalls a struct, like:
x, y, x2, y2
Actions are discarded, since no marshalling will be done for those when sending state data */
#define TO_AGENT_DT_EXPAND_MODEL_PROPERTY(x, y) ,x,y

#define TO_AGENT_DT_EXPAND_MODEL_REPORTED_PROPERTY(x, y) ,x,y

#define TO_AGENT_DT_EXPAND_MODEL_DESIRED_PROPERTY(x, y, ...) ,x,y

#define TO_AGENT_DT_EXPAND_MODEL_ACTION(...)

#define TO_AGENT_DT_EXPAND_MODEL_METHOD(...)

#define TO_AGENT_DT_EXPAND_ELEMENT_ARGS(N, ...) TO_AGENT_DT_EXPAND_##__VA_ARGS__

#define EXPAND_MODEL_ARGS(...) \
    MU_FOR_EACH_1_COUNTED(TO_AGENT_DT_EXPAND_ELEMENT_ARGS, __VA_ARGS__)

#define TO_AGENT_DATA_TYPE(name, ...) \
    static AGENT_DATA_TYPES_RESULT ToAGENT_DATA_TYPE_##name(AGENT_DATA_TYPE *destination, const name value) \
    { \
        AGENT_DATA_TYPES_RESULT result = AGENT_DATA_TYPES_OK; \
        size_t iMember = 0; \
        DEFINITION_THAT_CAN_SUSTAIN_A_COMMA_STEAL(phantomName, 1); \
        const char* memberNames[MU_IF(MU_DIV2(MU_C1(MU_COUNT_ARG(__VA_ARGS__))), MU_DIV2(MU_C1(MU_COUNT_ARG(__VA_ARGS__))), 1)] = { 0 }; \
        size_t memberCount = sizeof(memberNames) / sizeof(memberNames[0]); \
        (void)value; \
        if (memberCount == 0) \
        { \
            result = AGENT_DATA_TYPES_OK; \
        } \
        else \
        { \
            AGENT_DATA_TYPE members[sizeof(memberNames) / sizeof(memberNames[0])]; \
            DEFINITION_THAT_CAN_SUSTAIN_A_COMMA_STEAL(phantomName, 2); \
            MU_FOR_EACH_2(FIELD_AS_STRING, MU_EXPAND_TWICE(__VA_ARGS__)) \
            iMember = 0; \
            { \
                DEFINITION_THAT_CAN_SUSTAIN_A_COMMA_STEAL(phantomName, 3); \
                MU_FOR_EACH_2(CREATE_AGENT_DATA_TYPE, MU_EXPAND_TWICE(__VA_ARGS__)) \
                {DEFINITION_THAT_CAN_SUSTAIN_A_COMMA_STEAL(phantomName, 4); } \
                result = ((result == AGENT_DATA_TYPES_OK) && (Create_AGENT_DATA_TYPE_from_Members(destination, #name, sizeof(memberNames) / sizeof(memberNames[0]), memberNames, members) == AGENT_DATA_TYPES_OK)) \
                            ? AGENT_DATA_TYPES_OK \
                            : AGENT_DATA_TYPES_ERROR; \
                { \
                    size_t jMember; \
                    for (jMember = 0; jMember < iMember; jMember++) \
                    { \
                        Destroy_AGENT_DATA_TYPE(&members[jMember]); \
                    } \
                } \
            } \
        } \
        return result; \
    }

#define FIELD_AS_STRING(x,y) memberNames[iMember++] = #y;

#define REFLECTED_LIST_HEAD(name) \
    static const REFLECTED_DATA_FROM_DATAPROVIDER ALL_REFLECTED(name) = { &MU_C2(REFLECTED_, MU_C1(MU_DEC(__COUNTER__))) };
#define REFLECTED_STRUCT(name) \
    static const REFLECTED_SOMETHING MU_C2(REFLECTED_, MU_C1(MU_INC(__COUNTER__))) = { REFLECTION_STRUCT_TYPE,               &MU_C2(REFLECTED_, MU_C1(MU_DEC(MU_DEC(__COUNTER__)))), { {0}, {0}, {0}, {MU_TOSTRING(name)}, {0}, {0}, {0}, {0}} };
#define REFLECTED_FIELD(XstructName, XfieldType, XfieldName) \
    static const REFLECTED_SOMETHING MU_C2(REFLECTED_, MU_C1(MU_INC(__COUNTER__))) = { REFLECTION_FIELD_TYPE,                &MU_C2(REFLECTED_, MU_C1(MU_DEC(MU_DEC(__COUNTER__)))), { {0}, {0}, {0}, {0}, {MU_TOSTRING(XfieldName), MU_TOSTRING(XfieldType), MU_TOSTRING(XstructName)}, {0}, {0}, {0} } };
#define REFLECTED_MODEL(name) \
    static const REFLECTED_SOMETHING MU_C2(REFLECTED_, MU_C1(MU_INC(__COUNTER__))) = { REFLECTION_MODEL_TYPE,                &MU_C2(REFLECTED_, MU_C1(MU_DEC(MU_DEC(__COUNTER__)))), { {0}, {0}, {0}, {0}, {0}, {0}, {0}, {MU_TOSTRING(name)} } };
#define REFLECTED_PROPERTY(type, name, modelName) \
    static const REFLECTED_SOMETHING MU_C2(REFLECTED_, MU_C1(MU_INC(__COUNTER__))) = { REFLECTION_PROPERTY_TYPE,             &MU_C2(REFLECTED_, MU_C1(MU_DEC(MU_DEC(__COUNTER__)))), { {0}, {0}, {0}, {0}, {0}, {MU_TOSTRING(name), MU_TOSTRING(type), Create_AGENT_DATA_TYPE_From_Ptr_##modelName##name, offsetof(modelName, name), sizeof(type), MU_TOSTRING(modelName)}, {0}, {0} } };
#define REFLECTED_REPORTED_PROPERTY(type, name, modelName) \
    static const REFLECTED_SOMETHING MU_C2(REFLECTED_, MU_C1(MU_INC(__COUNTER__))) = { REFLECTION_REPORTED_PROPERTY_TYPE,    &MU_C2(REFLECTED_, MU_C1(MU_DEC(MU_DEC(__COUNTER__)))), { {0}, {0}, {MU_TOSTRING(name), MU_TOSTRING(type), Create_AGENT_DATA_TYPE_From_Ptr_##modelName##name, offsetof(modelName, name), sizeof(type), MU_TOSTRING(modelName)}, {0}, {0}, {0}, {0}, {0} } };


#define REFLECTED_DESIRED_PROPERTY_WITH_ON_DESIRED_PROPERTY_CHANGE(type, name, modelName, COUNTER, onDesiredPropertyChange) \
    static const REFLECTED_SOMETHING MU_C2(REFLECTED_, MU_C1(MU_INC(COUNTER))) =      { REFLECTION_DESIRED_PROPERTY_TYPE,     &MU_C2(REFLECTED_, MU_C1(MU_DEC(COUNTER))),         { {0}, {onDesiredPropertyChange, DesiredPropertyInitialize_##modelName##name, DesiredPropertyDeinitialize_##modelName##name, MU_TOSTRING(name), MU_TOSTRING(type), (int(*)(const AGENT_DATA_TYPE*, void*))FromAGENT_DATA_TYPE_##type, offsetof(modelName, name), sizeof(type), MU_TOSTRING(modelName)}, {0}, {0}, {0}, {0}, {0}, {0}} };

#define REFLECTED_DESIRED_PROPERTY(type, name, modelName, ...)                                                              \
    MU_IF(MU_COUNT_ARG(__VA_ARGS__),                                                                                              \
        MACRO_UTILS_DELAY(REFLECTED_DESIRED_PROPERTY_WITH_ON_DESIRED_PROPERTY_CHANGE)(type, name, modelName,__COUNTER__, __VA_ARGS__),  \
        MACRO_UTILS_DELAY(REFLECTED_DESIRED_PROPERTY_WITH_ON_DESIRED_PROPERTY_CHANGE)(type, name, modelName,MU_DEC(__COUNTER__), NULL)     \
    )                                                                                                                       \

#define REFLECTED_ACTION(name, argc, argv, fn, modelName) \
    static const REFLECTED_SOMETHING MU_C2(REFLECTED_, MU_C1(MU_INC(__COUNTER__))) = { REFLECTION_ACTION_TYPE,               &MU_C2(REFLECTED_, MU_C1(MU_DEC(MU_DEC(__COUNTER__)))), { {0}, {0}, {0}, {0}, {0}, {0}, {MU_TOSTRING(name), argc, argv, fn, MU_TOSTRING(modelName)}, {0}} };

#define REFLECTED_METHOD(name, argc, argv, fn, modelName) \
    static const REFLECTED_SOMETHING MU_C2(REFLECTED_, MU_C1(MU_INC(__COUNTER__))) = { REFLECTION_METHOD_TYPE,               &MU_C2(REFLECTED_, MU_C1(MU_DEC(MU_DEC(__COUNTER__)))), { {MU_TOSTRING(name), argc, argv, fn, MU_TOSTRING(modelName)}, {0}, {0}, {0}, {0}, {0}, {0}, {0}} };


#define REFLECTED_END_OF_LIST \
    static const REFLECTED_SOMETHING MU_C2(REFLECTED_, __COUNTER__) = {          REFLECTION_NOTHING,                   NULL,                                       { {0},{0}, {0}, {0}, {0}, {0}, {0}, {0}} };

#define EXPAND_MODEL_PROPERTY(type, name) MU_EXPAND_ARGS(MODEL_PROPERTY, type, name)

#define EXPAND_MODEL_REPORTED_PROPERTY(type, name) MU_EXPAND_ARGS(MODEL_REPORTED_PROPERTY, type, name)

#define EXPAND_MODEL_DESIRED_PROPERTY(type, name, ...) MU_EXPAND_ARGS(MODEL_DESIRED_PROPERTY, type, name, __VA_ARGS__)

#define EXPAND_MODEL_ACTION(...) MU_EXPAND_ARGS(MODEL_ACTION, __VA_ARGS__)

#define EXPAND_MODEL_METHOD(...) MU_EXPAND_ARGS(MODEL_METHOD, __VA_ARGS__)

#define BUILD_MODEL_STRUCT(elem) INSERT_FIELD_FOR_##elem

#define CREATE_MODEL_ENTITY(modelName, callType, ...) MU_EXPAND_ARGS(CREATE_##callType(modelName, __VA_ARGS__))
#define CREATE_SOMETHING(modelName, ...) MU_EXPAND_ARGS(CREATE_MODEL_ENTITY(modelName, __VA_ARGS__))
#define CREATE_ELEMENT(modelName, elem) MU_EXPAND_ARGS(CREATE_SOMETHING(modelName, MU_EXPAND_ARGS(EXPAND_##elem)))

#define CREATE_MODEL_ELEMENT(modelName, elem) MU_EXPAND_ARGS(CREATE_ELEMENT(modelName, elem))


#define CREATE_MODEL_ENTITY_GLOBAL_INITIALIZATION(modelName, callType, ...) MU_EXPAND_ARGS(CREATE_GLOBAL_INITIALIZE_##callType(modelName, __VA_ARGS__))
#define CREATE_SOMETHING_GLOBAL_INITIALIZATION(modelName, ...) MU_EXPAND_ARGS(CREATE_MODEL_ENTITY_GLOBAL_INITIALIZATION(modelName, __VA_ARGS__))
#define CREATE_ELEMENT_GLOBAL_INITIALIZATION(modelName, elem) MU_EXPAND_ARGS(CREATE_SOMETHING_GLOBAL_INITIALIZATION(modelName, MU_EXPAND_ARGS(EXPAND_##elem)))
#define CREATE_MODEL_ELEMENT_GLOBAL_INITIALIZE(modelName, elem) MU_EXPAND_ARGS(CREATE_ELEMENT_GLOBAL_INITIALIZATION(modelName, elem))

#define CREATE_MODEL_ENTITY_GLOBAL_DEINITIALIZATION(modelName, callType, ...) MU_EXPAND_ARGS(CREATE_GLOBAL_DEINITIALIZE_##callType(modelName, __VA_ARGS__))
#define CREATE_SOMETHING_GLOBAL_DEINITIALIZATION(modelName, ...) MU_EXPAND_ARGS(CREATE_MODEL_ENTITY_GLOBAL_DEINITIALIZATION(modelName, __VA_ARGS__))
#define CREATE_ELEMENT_GLOBAL_DEINITIALIZATION(modelName, elem) MU_EXPAND_ARGS(CREATE_SOMETHING_GLOBAL_DEINITIALIZATION(modelName, MU_EXPAND_ARGS(EXPAND_##elem)))
#define CREATE_MODEL_ELEMENT_GLOBAL_DEINITIALIZE(modelName, elem) MU_EXPAND_ARGS(CREATE_ELEMENT_GLOBAL_DEINITIALIZATION(modelName, elem))

#define INSERT_FIELD_INTO_STRUCT(x, y) x y;


#define INSERT_FIELD_FOR_MODEL_PROPERTY(type, name) INSERT_FIELD_INTO_STRUCT(type, name)
#define CREATE_GLOBAL_INITIALIZE_MODEL_PROPERTY(modelName, type, name) /*do nothing, this is written by user*/
#define CREATE_GLOBAL_DEINITIALIZE_MODEL_PROPERTY(modelName, type, name) /*do nothing, this is user's stuff*/

/*REPORTED_PROPERTY is not different than regular WITH_DATA*/
#define INSERT_FIELD_FOR_MODEL_REPORTED_PROPERTY(type, name) INSERT_FIELD_INTO_STRUCT(type, name)
#define CREATE_GLOBAL_INITIALIZE_MODEL_REPORTED_PROPERTY(modelName, type,name) GlobalInitialize_##type((char*)destination+offsetof(modelName, name));
#define CREATE_GLOBAL_DEINITIALIZE_MODEL_REPORTED_PROPERTY(modelName, type,name) GlobalDeinitialize_##type((char*)destination+offsetof(modelName, name));

/*DESIRED_PROPERTY is not different than regular WITH_DATA*/
#define INSERT_FIELD_FOR_MODEL_DESIRED_PROPERTY(type, name, ...) INSERT_FIELD_INTO_STRUCT(type, name)
#define CREATE_GLOBAL_INITIALIZE_MODEL_DESIRED_PROPERTY(modelName, type, name, ...) /*do nothing*/
#define CREATE_GLOBAL_DEINITIALIZE_MODEL_DESIRED_PROPERTY(modelName, type, name, ...) /*do nothing*/

#define INSERT_FIELD_FOR_MODEL_ACTION(name, ...) /* action isn't a part of the model struct */
#define INSERT_FIELD_FOR_MODEL_METHOD(name, ...) /* method isn't a part of the model struct */

#define CREATE_GLOBAL_INITIALIZE_MODEL_ACTION(...) /*do nothing*/
#define CREATE_GLOBAL_DEINITIALIZE_MODEL_ACTION(...) /*do nothing*/

#define CREATE_GLOBAL_INITIALIZE_MODEL_METHOD(...) /*do nothing*/
#define CREATE_GLOBAL_DEINITIALIZE_MODEL_METHOD(...) /*do nothing*/

#define CREATE_MODEL_PROPERTY(modelName, type, name) \
    IMPL_PROPERTY(type, name, modelName)

#define CREATE_MODEL_REPORTED_PROPERTY(modelName, type, name) \
    IMPL_REPORTED_PROPERTY(type, name, modelName)

#define CREATE_MODEL_DESIRED_PROPERTY(modelName, type, name, ...) \
    IMPL_DESIRED_PROPERTY(type, name, modelName, __VA_ARGS__)

#define IMPL_PROPERTY(propertyType, propertyName, modelName) \
    static int Create_AGENT_DATA_TYPE_From_Ptr_##modelName##propertyName(void* param, AGENT_DATA_TYPE* dest) \
    { \
        return MU_C1(ToAGENT_DATA_TYPE_##propertyType)(dest, *(propertyType*)param); \
    } \
    REFLECTED_PROPERTY(propertyType, propertyName, modelName)

#define IMPL_REPORTED_PROPERTY(propertyType, propertyName, modelName) \
    static int Create_AGENT_DATA_TYPE_From_Ptr_##modelName##propertyName(void* param, AGENT_DATA_TYPE* dest) \
    { \
        return MU_C1(ToAGENT_DATA_TYPE_##propertyType)(dest, *(propertyType*)param); \
    } \
    REFLECTED_REPORTED_PROPERTY(propertyType, propertyName, modelName)

#define IMPL_DESIRED_PROPERTY(propertyType, propertyName, modelName, ...)           \
    static void DesiredPropertyInitialize_##modelName##propertyName(void* destination)                             \
    {                                                                                                   \
        GlobalInitialize_##propertyType(destination);                                                   \
    }                                                                                                   \
    static void DesiredPropertyDeinitialize_##modelName##propertyName(void* destination)                           \
    {                                                                                                   \
       GlobalDeinitialize_##propertyType(destination);                                                  \
    }                                                                                                   \
    REFLECTED_DESIRED_PROPERTY(propertyType, propertyName, modelName, __VA_ARGS__)          \

#define CREATE_MODEL_ACTION(modelName, actionName, ...) \
    DEFINITION_THAT_CAN_SUSTAIN_A_COMMA_STEAL(modelName##actionName, 1); \
    EXECUTE_COMMAND_RESULT actionName (modelName* device MU_FOR_EACH_2(DEFINE_FUNCTION_PARAMETER, __VA_ARGS__)); \
    static EXECUTE_COMMAND_RESULT MU_C2(actionName, WRAPPER)(void* device, size_t ParameterCount, const AGENT_DATA_TYPE* values); \
    /*for macro purposes, this array always has at least 1 element*/ \
    /*Codes_SRS_SERIALIZER_99_043:[ It is valid for a method not to have any parameters.]*/ \
    DEFINITION_THAT_CAN_SUSTAIN_A_COMMA_STEAL(actionName, 1); \
    static const WRAPPER_ARGUMENT MU_C2(actionName, WRAPPERARGUMENTS)[MU_DIV2(MU_INC(MU_INC(MU_COUNT_ARG(__VA_ARGS__))))] = { MU_FOR_EACH_2_COUNTED(MAKE_WRAPPER_ARGUMENT, __VA_ARGS__) MU_IFCOMMA(MU_INC(MU_INC(MU_COUNT_ARG(__VA_ARGS__)))) {0} }; \
    REFLECTED_ACTION(actionName, MU_DIV2(MU_COUNT_ARG(__VA_ARGS__)), MU_C2(actionName, WRAPPERARGUMENTS), MU_C2(actionName, WRAPPER), modelName) \
    /*Codes_SRS_SERIALIZER_99_040:[ In addition to declaring the function, DECLARE_IOT_METHOD shall provide a definition for a wrapper that takes as parameters a size_t parameterCount and const AGENT_DATA_TYPE*.] */ \
    /*Codes_SRS_SERIALIZER_99_041:[ This wrapper shall convert all the arguments to predefined types and then call the function written by the data provider developer.]*/ \
    static EXECUTE_COMMAND_RESULT MU_C2(actionName, WRAPPER)(void* device, size_t ParameterCount, const AGENT_DATA_TYPE* values) \
    { \
        EXECUTE_COMMAND_RESULT result; \
        DEFINITION_THAT_CAN_SUSTAIN_A_COMMA_STEAL(actionName, 2); \
        /*Codes_SRS_SERIALIZER_99_045:[ If the number of passed parameters doesn't match the number of declared parameters, wrapper execution shall fail and return DATA_PROVIDER_INVALID_ARG;]*/ \
        if(ParameterCount != MU_DIV2(MU_COUNT_ARG(__VA_ARGS__))) \
        { \
            result = EXECUTE_COMMAND_ERROR; \
        } \
        else \
        { \
            /*the below line takes care of initialized but not referenced parameter warning*/ \
            DEFINITION_THAT_CAN_SUSTAIN_A_COMMA_STEAL(actionName, 3); \
            MU_IF(MU_DIV2(MU_COUNT_ARG(__VA_ARGS__)), size_t iParameter = 0;, ) \
            DEFINITION_THAT_CAN_SUSTAIN_A_COMMA_STEAL(actionName, 4); \
            /*the below line takes care of an unused parameter when values is really never questioned*/ \
            DEFINITION_THAT_CAN_SUSTAIN_A_COMMA_STEAL(actionName, 5); \
            MU_FOR_EACH_2(DEFINE_LOCAL_PARAMETER, __VA_ARGS__) \
            DEFINITION_THAT_CAN_SUSTAIN_A_COMMA_STEAL(actionName, 6); \
            MU_IF(MU_DIV2(MU_COUNT_ARG(__VA_ARGS__)), , (void)values;) \
            { \
               DEFINITION_THAT_CAN_SUSTAIN_A_COMMA_STEAL(actionName, 7); \
            } \
            MU_FOR_EACH_2_KEEP_1(START_BUILD_LOCAL_PARAMETER, EXECUTE_COMMAND_ERROR, __VA_ARGS__) \
            { \
               DEFINITION_THAT_CAN_SUSTAIN_A_COMMA_STEAL(actionName, 8); \
            } \
            result = actionName((modelName*)device MU_FOR_EACH_2(PUSH_LOCAL_PARAMETER, __VA_ARGS__)); \
            MU_FOR_EACH_2_REVERSE(END_BUILD_LOCAL_PARAMETER, __VA_ARGS__) \
        } \
        return result; \
    }

#define CREATE_MODEL_METHOD(modelName, methodName, ...) \
    DEFINITION_THAT_CAN_SUSTAIN_A_COMMA_STEAL(modelName##methodName, 1); \
    METHODRETURN_HANDLE methodName (modelName* device MU_FOR_EACH_2(DEFINE_FUNCTION_PARAMETER, __VA_ARGS__)); \
    static METHODRETURN_HANDLE MU_C2(methodName, WRAPPER)(void* device, size_t ParameterCount, const AGENT_DATA_TYPE* values); \
    /*for macro purposes, this array always has at least 1 element*/ \
    /*Codes_SRS_SERIALIZER_H_02_030: [ It is valid for a method function not to have any parameters. ]*/ \
    DEFINITION_THAT_CAN_SUSTAIN_A_COMMA_STEAL(methodName, 1); \
    static const WRAPPER_ARGUMENT MU_C2(methodName, WRAPPERARGUMENTS)[MU_DIV2(MU_INC(MU_INC(MU_COUNT_ARG(__VA_ARGS__))))] = { MU_FOR_EACH_2_COUNTED(MAKE_WRAPPER_ARGUMENT, __VA_ARGS__) MU_IFCOMMA(MU_INC(MU_INC(MU_COUNT_ARG(__VA_ARGS__)))) {0} }; \
    REFLECTED_METHOD(methodName, MU_DIV2(MU_COUNT_ARG(__VA_ARGS__)), MU_C2(methodName, WRAPPERARGUMENTS), MU_C2(methodName, WRAPPER), modelName) \
    /*Codes_SRS_SERIALIZER_H_02_034: [ WITH_METHOD shall result in the declaration of a conversion function with the prototype METHODRETURN_HANDLE nameWRAPPER(size_t ParameterCount, const AGENT_DATA_TYPE* values)' ]*/ \
    /*Codes_SRS_SERIALIZER_H_02_031: [ The function shall convert the input arguments to the types declared in the method parameter list and then call the user-defined method function. ]*/ \
    static METHODRETURN_HANDLE MU_C2(methodName, WRAPPER)(void* device, size_t ParameterCount, const AGENT_DATA_TYPE* values) \
    { \
        METHODRETURN_HANDLE result; \
        DEFINITION_THAT_CAN_SUSTAIN_A_COMMA_STEAL(methodName, 2); \
        /*Codes_SRS_SERIALIZER_H_02_032: [ If the number of arguments passed to the conversion function does not match the expected count, the function shall return DATAPROVIDER_INVALID_ARG. ]*/ \
        if(ParameterCount != MU_DIV2(MU_COUNT_ARG(__VA_ARGS__))) \
        { \
            LogError("expected parameter count (%lu) does not match the actual parameter count (%lu)", (unsigned long)ParameterCount, (unsigned long)MU_COUNT_ARG(__VA_ARGS__)); \
            result = NULL; \
        } \
        else \
        { \
            /*the below line takes care of initialized but not referenced parameter warning*/ \
            DEFINITION_THAT_CAN_SUSTAIN_A_COMMA_STEAL(methodName, 3); \
            MU_IF(MU_DIV2(MU_COUNT_ARG(__VA_ARGS__)), size_t iParameter = 0;, ) \
            DEFINITION_THAT_CAN_SUSTAIN_A_COMMA_STEAL(methodName, 4); \
            /*the below line takes care of an unused parameter when values is really never questioned*/ \
            DEFINITION_THAT_CAN_SUSTAIN_A_COMMA_STEAL(methodName, 5); \
            MU_FOR_EACH_2(DEFINE_LOCAL_PARAMETER, __VA_ARGS__) \
            DEFINITION_THAT_CAN_SUSTAIN_A_COMMA_STEAL(methodName, 6); \
            MU_IF(MU_DIV2(MU_COUNT_ARG(__VA_ARGS__)), , (void)values;) \
            { \
               DEFINITION_THAT_CAN_SUSTAIN_A_COMMA_STEAL(actionName, 7); \
            } \
            MU_FOR_EACH_2_KEEP_1(START_BUILD_LOCAL_PARAMETER, NULL,__VA_ARGS__) \
            { \
               DEFINITION_THAT_CAN_SUSTAIN_A_COMMA_STEAL(actionName, 8); \
            } \
            result = methodName((modelName*)device MU_FOR_EACH_2(PUSH_LOCAL_PARAMETER, __VA_ARGS__)); \
            MU_FOR_EACH_2_REVERSE(END_BUILD_LOCAL_PARAMETER, __VA_ARGS__) \
        } \
        return result; \
    }

#define CREATE_AGENT_DATA_TYPE(type, name) \
    result = (( result==AGENT_DATA_TYPES_OK) && (ToAGENT_DATA_TYPE_##type( &(members[iMember]), value.name) == AGENT_DATA_TYPES_OK))?AGENT_DATA_TYPES_OK:AGENT_DATA_TYPES_ERROR; \
    iMember+= ((result==AGENT_DATA_TYPES_OK)?1:0);

#define BUILD_DESTINATION_FIELD(type, name) \
    if(result == AGENT_DATA_TYPES_OK) \
    { \
        size_t i; \
        bool wasFieldConverted = false; \
        for (i = 0; i < source->value.edmComplexType.nMembers; i++) \
        { \
            /*the name of the field of the complex type must match the name of the field of the structure (parameter name here)*/ \
            if (strcmp(source->value.edmComplexType.fields[i].fieldName, MU_TOSTRING(name)) == 0) \
            { /*Codes_SRS_SERIALIZER_99_017:[ These types can either be one of the types mentioned in WITH_DATA or it can be a type introduced by a previous DECLARE_STRUCT.]*/ \
                wasFieldConverted = (MU_C2(FromAGENT_DATA_TYPE_, type)(source->value.edmComplexType.fields[i].value, &(destination->name)) == AGENT_DATA_TYPES_OK); \
                break; \
            } \
        } \
        result = (wasFieldConverted == true)? AGENT_DATA_TYPES_OK: AGENT_DATA_TYPES_INVALID_ARG; \
    } \
    else \
    { \
        /*fallthrough*/ \
    }

#define UNBUILD_DESTINATION_FIELD(value, type, name) \
    MU_C2(destroyLocalParameter, type)(&(value->name));


#define ADDRESS_MACRO(x) ,&x

#define KEEP_FIRST_(X, ...) X
#ifdef _MSC_VER
#define KEEP_FIRST(X) KEEP_FIRST_ LPAREN X)
#else
#define KEEP_FIRST(X) KEEP_FIRST_(X)
#endif

#define PROMOTIONMAP_float double, double
#define PROMOTIONMAP_int8_t int, int
#define PROMOTIONMAP_uint8_t int, int
#define PROMOTIONMAP_int16_t int, int
#define PROMOTIONMAP__Bool int, int
#define PROMOTIONMAP_bool int, int

#define CASTMAP_float (float), (float)
#define CASTMAP_int8_t (int8_t), (int8_t)
#define CASTMAP_uint8_t (uint8_t), (uint8_t)
#define CASTMAP_int16_t (int16_t), (int16_t)
#define CASTMAP__Bool 0!=, 0!=
#define CASTMAP_bool 0!=, 0!=

#define EMPTY_TOKEN

#define ANOTHERIF(x) MU_C2(ANOTHERIF,x)
#define ANOTHERIF0(a,b) a
#define ANOTHERIF1(a,b) b
#define ANOTHERIF2(a,b) b
#define ANOTHERIF3(a,b) b
#define ANOTHERIF4(a,b) b
#define ANOTHERIF5(a,b) b
#define ANOTHERIF6(a,b) b
#define ANOTHERIF7(a,b) b
#define ANOTHERIF8(a,b) b
#define ANOTHERIF9(a,b) b
#define ANOTHERIF10(a,b) b
#define ANOTHERIF11(a,b) b
#define ANOTHERIF12(a,b) b

#define MAP_PROMOTED_TYPE(X) ANOTHERIF(MU_DEC(MU_COUNT_ARG(PROMOTIONMAP_##X))) (X, KEEP_FIRST(PROMOTIONMAP_##X))
#define MAP_CAST_TYPE(X) ANOTHERIF(MU_DEC(MU_COUNT_ARG(CASTMAP_##X)))    (EMPTY_TOKEN, KEEP_FIRST(CASTMAP_##X)  )

#define MU_IFCOMMA(N) MU_C2(MU_IFCOMMA_, N)
#define MU_IFCOMMA_0
#define MU_IFCOMMA_2
#define MU_IFCOMMA_4 ,
#define MU_IFCOMMA_6 ,
#define MU_IFCOMMA_8 ,
#define MU_IFCOMMA_10 ,
#define MU_IFCOMMA_12 ,
#define MU_IFCOMMA_14 ,
#define MU_IFCOMMA_16 ,
#define MU_IFCOMMA_18 ,
#define MU_IFCOMMA_20 ,
#define MU_IFCOMMA_22 ,
#define MU_IFCOMMA_24 ,
#define MU_IFCOMMA_26 ,
#define MU_IFCOMMA_28 ,
#define MU_IFCOMMA_30 ,
#define MU_IFCOMMA_32 ,
#define MU_IFCOMMA_34 ,
#define MU_IFCOMMA_36 ,
#define MU_IFCOMMA_38 ,
#define MU_IFCOMMA_40 ,
#define MU_IFCOMMA_42 ,
#define MU_IFCOMMA_44 ,
#define MU_IFCOMMA_46 ,
#define MU_IFCOMMA_48 ,
#define MU_IFCOMMA_50 ,
#define MU_IFCOMMA_52 ,
#define MU_IFCOMMA_54 ,
#define MU_IFCOMMA_56 ,
#define MU_IFCOMMA_58 ,
#define MU_IFCOMMA_60 ,
#define MU_IFCOMMA_62 ,
#define MU_IFCOMMA_64 ,
#define MU_IFCOMMA_66 ,
#define MU_IFCOMMA_68 ,
#define MU_IFCOMMA_70 ,
#define MU_IFCOMMA_72 ,
#define MU_IFCOMMA_74 ,
#define MU_IFCOMMA_76 ,
#define MU_IFCOMMA_78 ,
#define MU_IFCOMMA_80 ,
#define MU_IFCOMMA_82 ,
#define MU_IFCOMMA_84 ,
#define MU_IFCOMMA_86 ,
#define MU_IFCOMMA_88 ,
#define MU_IFCOMMA_90 ,
#define MU_IFCOMMA_92 ,
#define MU_IFCOMMA_94 ,
#define MU_IFCOMMA_96 ,
#define MU_IFCOMMA_98 ,
#define MU_IFCOMMA_100 ,
#define MU_IFCOMMA_102 ,
#define MU_IFCOMMA_104 ,
#define MU_IFCOMMA_106 ,
#define MU_IFCOMMA_108 ,
#define MU_IFCOMMA_110 ,
#define MU_IFCOMMA_112 ,
#define MU_IFCOMMA_114 ,
#define MU_IFCOMMA_116 ,
#define MU_IFCOMMA_118 ,
#define MU_IFCOMMA_120 ,
#define MU_IFCOMMA_122 ,
#define MU_IFCOMMA_124 ,
#define MU_IFCOMMA_126 ,
#define MU_IFCOMMA_128 ,

#define DEFINE_LOCAL_PARAMETER(type, name) type MU_C2(name,_local); GlobalInitialize_##type(& MU_C2(name, _local));

#define START_BUILD_LOCAL_PARAMETER(errorWhenItFails, type, name) \
    if (MU_C2(FromAGENT_DATA_TYPE_, type)(&values[iParameter], &MU_C2(name, _local)) != AGENT_DATA_TYPES_OK) \
    { \
        /*Codes_SRS_SERIALIZER_99_046:[ If the types of the parameters do not match the declared types, DATAPROVIDER_INVALID_ARG shall be returned.]*/ \
        result = errorWhenItFails; \
    }\
    else \
    { \
        iParameter++;

#define END_BUILD_LOCAL_PARAMETER(type, name) \
    (void)MU_C2(destroyLocalParameter, type)(&MU_C2(name, _local)); \
    }

/*The following constructs have been devised to work around the precompiler bug of Visual Studio 2005, version 14.00.50727.42*/
/* The bug is explained in https://connect.microsoft.com/VisualStudio/feedback/details/278752/comma-missing-when-using-va-args */
/*A short description is: preprocessor is mysteriously eating commas ','.
In order to feed the appetite of the preprocessor, several constructs have
been devised that can sustain a missing ',' while still compiling and while still doing nothing
and while hopefully being eliminated from the code based on "doesn't do anything" so no code size penalty
*/

/*the reason why all these constructs work is:
if two strings separated by a comma will lose the comma (myteriously) then they will become just one string:
"a", "b" ------Preprocessor------> "a" "b" -----Compiler----> "ab"
*/

#define LOTS_OF_COMMA_TO_BE_EATEN /*there were witnesses where as many as THREE commas have been eaten!*/ \
"0" "1", "2", "3", "4", "5", "6", "7", "8", "9"
#define DEFINITION_THAT_CAN_SUSTAIN_A_COMMA_STEAL(name, instance) static const char* eatThese_COMMA_##name##instance[] = {LOTS_OF_COMMA_TO_BE_EATEN}

#define PUSH_LOCAL_PARAMETER(type, name) , MU_C2(name, _local)
#define DEFINE_FUNCTION_PARAMETER(type, name) , type name
#define MAKE_WRAPPER_ARGUMENT(N, type, name) {MU_TOSTRING(type), MU_TOSTRING(name)} MU_IFCOMMA(N)

/*Codes_SRS_SERIALIZER_99_019:[ Create_AGENT_DATA_TYPE_from_DOUBLE]*/
/*Codes_SRS_SERIALIZER_99_004:[ The propertyType can be any of the following data types: double]*/
static AGENT_DATA_TYPES_RESULT MU_C2(ToAGENT_DATA_TYPE_, double)(AGENT_DATA_TYPE* dest, double source)
{
    return Create_AGENT_DATA_TYPE_from_DOUBLE(dest, source);
}

static AGENT_DATA_TYPES_RESULT MU_C2(FromAGENT_DATA_TYPE_, double)(const AGENT_DATA_TYPE* agentData, double* dest)
{
    AGENT_DATA_TYPES_RESULT result;
    if (agentData->type != EDM_DOUBLE_TYPE)
    {
        result = AGENT_DATA_TYPES_INVALID_ARG;
    }
    else
    {
        *dest = agentData->value.edmDouble.value;
        result = AGENT_DATA_TYPES_OK;
    }
    return result;
}

static void MU_C2(GlobalInitialize_, double)(void* dest)
{
    *(double*)dest = 0.0;
}

static void MU_C2(GlobalDeinitialize_, double)(void* dest)
{
    (void)(dest);
}

/*Codes_SRS_SERIALIZER_99_021:[ Create_AGENT_DATA_TYPE_from_FLOAT]*/
/*Codes_SRS_SERIALIZER_99_006:[ float]*/
static AGENT_DATA_TYPES_RESULT MU_C2(ToAGENT_DATA_TYPE_, float)(AGENT_DATA_TYPE* dest, float source)
{
    return Create_AGENT_DATA_TYPE_from_FLOAT(dest, source);
}

static AGENT_DATA_TYPES_RESULT MU_C2(FromAGENT_DATA_TYPE_, float)(const AGENT_DATA_TYPE* agentData, float* dest)
{
    AGENT_DATA_TYPES_RESULT result;
    if (agentData->type != EDM_SINGLE_TYPE)
    {
        result = AGENT_DATA_TYPES_INVALID_ARG;
    }
    else
    {
        *dest = agentData->value.edmSingle.value;
        result = AGENT_DATA_TYPES_OK;
    }
    return result;
}

static void MU_C2(GlobalInitialize_, float)(void* dest)
{
    *(float*)dest = 0.0f;
}

static void MU_C2(GlobalDeinitialize_, float)(void* dest)
{
    (void)(dest);
}


/*Codes_SRS_SERIALIZER_99_020:[ Create_AGENT_DATA_TYPE_from_SINT32]*/
/*Codes_SRS_SERIALIZER_99_005:[ int], */
static AGENT_DATA_TYPES_RESULT MU_C2(ToAGENT_DATA_TYPE_, int)(AGENT_DATA_TYPE* dest, int source)
{
    return Create_AGENT_DATA_TYPE_from_SINT32(dest, source);
}

static AGENT_DATA_TYPES_RESULT MU_C2(FromAGENT_DATA_TYPE_, int)(const AGENT_DATA_TYPE* agentData, int* dest)
{
    AGENT_DATA_TYPES_RESULT result;
    if (agentData->type != EDM_INT32_TYPE)
    {
        result = AGENT_DATA_TYPES_INVALID_ARG;
    }
    else
    {
        *dest = agentData->value.edmInt32.value;
        result = AGENT_DATA_TYPES_OK;
    }
    return result;
}

static void MU_C2(GlobalInitialize_, int)(void* dest)
{
    *(int*)dest = 0;
}

static void MU_C2(GlobalDeinitialize_, int)(void* dest)
{
    (void)(dest);
}

/*Codes_SRS_SERIALIZER_99_022:[ Create_AGENT_DATA_TYPE_from_SINT64]*/
/*Codes_SRS_SERIALIZER_99_007:[ long]*/
static AGENT_DATA_TYPES_RESULT MU_C2(ToAGENT_DATA_TYPE_, long)(AGENT_DATA_TYPE* dest, long source)
{
    return Create_AGENT_DATA_TYPE_from_SINT64(dest, source);
}

static AGENT_DATA_TYPES_RESULT MU_C2(FromAGENT_DATA_TYPE_, long)(const AGENT_DATA_TYPE* agentData, long* dest)
{
    AGENT_DATA_TYPES_RESULT result;
    if (agentData->type != EDM_INT64_TYPE)
    {
        result = AGENT_DATA_TYPES_INVALID_ARG;
    }
    else
    {
        *dest = (long)agentData->value.edmInt64.value;
        result = AGENT_DATA_TYPES_OK;
    }
    return result;
}

static void MU_C2(GlobalInitialize_, long)(void* dest)
{
    *(long*)dest = 0;
}

static void MU_C2(GlobalDeinitialize_, long)(void* dest)
{
    (void)(dest);
}


/*Codes_SRS_SERIALIZER_99_023:[ Create_AGENT_DATA_TYPE_from_SINT8]*/
/*Codes_SRS_SERIALIZER_99_008:[ int8_t]*/
static AGENT_DATA_TYPES_RESULT MU_C2(ToAGENT_DATA_TYPE_, int8_t)(AGENT_DATA_TYPE* dest, int8_t source)
{
    return Create_AGENT_DATA_TYPE_from_SINT8(dest, source);
}

static AGENT_DATA_TYPES_RESULT MU_C2(FromAGENT_DATA_TYPE_, int8_t)(const AGENT_DATA_TYPE* agentData, int8_t* dest)
{
    AGENT_DATA_TYPES_RESULT result;
    if (agentData->type != EDM_SBYTE_TYPE)
    {
        result = AGENT_DATA_TYPES_INVALID_ARG;
    }
    else
    {
        *dest = agentData->value.edmSbyte.value;
        result = AGENT_DATA_TYPES_OK;
    }
    return result;
}

static void MU_C2(GlobalInitialize_, int8_t)(void* dest)
{
    *(int8_t*)dest = 0;
}

static void MU_C2(GlobalDeinitialize_, int8_t)(void* dest)
{
    (void)(dest);
}

/*Codes_SRS_SERIALIZER_99_024:[ Create_AGENT_DATA_TYPE_from_UINT8]*/
/*Codes_SRS_SERIALIZER_99_009:[ uint8_t]*/
static AGENT_DATA_TYPES_RESULT MU_C2(ToAGENT_DATA_TYPE_, uint8_t)(AGENT_DATA_TYPE* dest, uint8_t source)
{
    return Create_AGENT_DATA_TYPE_from_UINT8(dest, source);
}

static AGENT_DATA_TYPES_RESULT MU_C2(FromAGENT_DATA_TYPE_, uint8_t)(const AGENT_DATA_TYPE* agentData, uint8_t* dest)
{
    AGENT_DATA_TYPES_RESULT result;
    if (agentData->type != EDM_BYTE_TYPE)
    {
        result = AGENT_DATA_TYPES_INVALID_ARG;
    }
    else
    {
        *dest = agentData->value.edmByte.value;
        result = AGENT_DATA_TYPES_OK;
    }
    return result;
}

static void MU_C2(GlobalInitialize_, uint8_t)(void* dest)
{
    *(uint8_t*)dest = 0;
}

static void MU_C2(GlobalDeinitialize_, uint8_t)(void* dest)
{
    (void)(dest);
}


/*Codes_SRS_SERIALIZER_99_025:[ Create_AGENT_DATA_TYPE_from_SINT16]*/
/*Codes_SRS_SERIALIZER_99_010:[ int16_t]*/
static AGENT_DATA_TYPES_RESULT MU_C2(ToAGENT_DATA_TYPE_, int16_t)(AGENT_DATA_TYPE* dest, int16_t source)
{
    return Create_AGENT_DATA_TYPE_from_SINT16(dest, source);
}

static AGENT_DATA_TYPES_RESULT MU_C2(FromAGENT_DATA_TYPE_, int16_t)(const AGENT_DATA_TYPE* agentData, int16_t* dest)
{
    AGENT_DATA_TYPES_RESULT result;
    if (agentData->type != EDM_INT16_TYPE)
    {
        result = AGENT_DATA_TYPES_INVALID_ARG;
    }
    else
    {
        *dest = agentData->value.edmInt16.value;
        result = AGENT_DATA_TYPES_OK;
    }
    return result;
}

static void MU_C2(GlobalInitialize_, int16_t)(void* dest)
{
    *(int16_t*)dest = 0;
}

static void MU_C2(GlobalDeinitialize_, int16_t)(void* dest)
{
    (void)(dest);
}

/*Codes_SRS_SERIALIZER_99_026:[ Create_AGENT_DATA_TYPE_from_SINT32]*/
/*Codes_SRS_SERIALIZER_99_011:[ int32_t]*/
static AGENT_DATA_TYPES_RESULT MU_C2(ToAGENT_DATA_TYPE_, int32_t)(AGENT_DATA_TYPE* dest, int32_t source)
{
    return Create_AGENT_DATA_TYPE_from_SINT32(dest, source);
}

static AGENT_DATA_TYPES_RESULT MU_C2(FromAGENT_DATA_TYPE_, int32_t)(const AGENT_DATA_TYPE* agentData, int32_t* dest)
{
    AGENT_DATA_TYPES_RESULT result;
    if (agentData->type != EDM_INT32_TYPE)
    {
        result = AGENT_DATA_TYPES_INVALID_ARG;
    }
    else
    {
        *dest = agentData->value.edmInt32.value;
        result = AGENT_DATA_TYPES_OK;
    }
    return result;
}

static void MU_C2(GlobalInitialize_, int32_t)(void* dest)
{
    *(int32_t*)dest = 0;
}

static void MU_C2(GlobalDeinitialize_, int32_t)(void* dest)
{
    (void)(dest);
}

/*Codes_SRS_SERIALIZER_99_027:[ Create_AGENT_DATA_TYPE_from_SINT64]*/
/*Codes_SRS_SERIALIZER_99_012:[ int64_t]*/
static AGENT_DATA_TYPES_RESULT MU_C2(ToAGENT_DATA_TYPE_, int64_t)(AGENT_DATA_TYPE* dest, int64_t source)
{
    return Create_AGENT_DATA_TYPE_from_SINT64(dest, source);
}

static AGENT_DATA_TYPES_RESULT MU_C2(FromAGENT_DATA_TYPE_, int64_t)(const AGENT_DATA_TYPE* agentData, int64_t* dest)
{
    AGENT_DATA_TYPES_RESULT result;
    if (agentData->type != EDM_INT64_TYPE)
    {
        result = AGENT_DATA_TYPES_INVALID_ARG;
    }
    else
    {
        *dest = agentData->value.edmInt64.value;
        result = AGENT_DATA_TYPES_OK;
    }
    return result;
}

static void MU_C2(GlobalInitialize_, int64_t)(void* dest)
{
    *(int64_t*)dest = 0;
}

static void MU_C2(GlobalDeinitialize_, int64_t)(void* dest)
{
    (void)(dest);
}

/*Codes_SRS_SERIALIZER_99_013:[ bool]*/
static AGENT_DATA_TYPES_RESULT MU_C2(ToAGENT_DATA_TYPE_, bool)(AGENT_DATA_TYPE* dest, bool source)
{
    return Create_EDM_BOOLEAN_from_int(dest, source == true);
}

static AGENT_DATA_TYPES_RESULT MU_C2(FromAGENT_DATA_TYPE_, bool)(const AGENT_DATA_TYPE* agentData, bool* dest)
{
    AGENT_DATA_TYPES_RESULT result;
    if (agentData->type != EDM_BOOLEAN_TYPE)
    {
        result = AGENT_DATA_TYPES_INVALID_ARG;
    }
    else
    {
        *dest = (agentData->value.edmBoolean.value == EDM_TRUE) ? true : false;
        result = AGENT_DATA_TYPES_OK;
    }
    return result;
}

static void MU_C2(GlobalInitialize_, bool)(void* dest)
{
    *(bool*)dest = false;
}

static void MU_C2(GlobalDeinitialize_, bool)(void* dest)
{
    (void)(dest);
}

/*Codes_SRS_SERIALIZER_99_014:[ ascii_char_ptr]*/
static AGENT_DATA_TYPES_RESULT MU_C2(ToAGENT_DATA_TYPE_, ascii_char_ptr)(AGENT_DATA_TYPE* dest, ascii_char_ptr source)
{
    return Create_AGENT_DATA_TYPE_from_charz(dest, source);
}

static AGENT_DATA_TYPES_RESULT MU_C2(FromAGENT_DATA_TYPE_, ascii_char_ptr)(const AGENT_DATA_TYPE* agentData, ascii_char_ptr* dest)
{
    AGENT_DATA_TYPES_RESULT result;
    if (agentData->type != EDM_STRING_TYPE)
    {
        result = AGENT_DATA_TYPES_INVALID_ARG;
    }
    else
    {
        if (*dest != NULL)
        {
            free(*dest);
            *dest = NULL;
        }

        if (mallocAndStrcpy_s(dest, agentData->value.edmString.chars) != 0)
        {
            LogError("failure in mallocAndStrcpy_s");
            result = AGENT_DATA_TYPES_ERROR;
        }
        else
        {
            result = AGENT_DATA_TYPES_OK;
        }

    }
    return result;
}

static void MU_C2(GlobalInitialize_, ascii_char_ptr)(void* dest)
{
    *(ascii_char_ptr*)dest = NULL;
}

static void MU_C2(GlobalDeinitialize_, ascii_char_ptr)(void* dest)
{
    if (*(ascii_char_ptr*)dest != NULL)
    {
        free(*(ascii_char_ptr*)dest);
    }
}

static AGENT_DATA_TYPES_RESULT MU_C2(ToAGENT_DATA_TYPE_, ascii_char_ptr_no_quotes)(AGENT_DATA_TYPE* dest, ascii_char_ptr_no_quotes source)
{
    return Create_AGENT_DATA_TYPE_from_charz_no_quotes(dest, source);
}

static AGENT_DATA_TYPES_RESULT MU_C2(FromAGENT_DATA_TYPE_, ascii_char_ptr_no_quotes)(const AGENT_DATA_TYPE* agentData, ascii_char_ptr_no_quotes* dest)
{
    AGENT_DATA_TYPES_RESULT result;
    if (agentData->type != EDM_STRING_NO_QUOTES_TYPE)
    {
        result = AGENT_DATA_TYPES_INVALID_ARG;
    }
    else
    {
        if (*dest != NULL)
        {
            free(*dest);
            *dest = NULL;
        }

        if (mallocAndStrcpy_s(dest, agentData->value.edmStringNoQuotes.chars) != 0)
        {
            LogError("failure in mallocAndStrcpy_s");
            result = AGENT_DATA_TYPES_ERROR;
        }
        else
        {
            result = AGENT_DATA_TYPES_OK;
        }
    }
    return result;
}

static void MU_C2(GlobalInitialize_, ascii_char_ptr_no_quotes)(void* dest)
{
    *(ascii_char_ptr_no_quotes*)dest = NULL;
}

static void MU_C2(GlobalDeinitialize_, ascii_char_ptr_no_quotes)(void* dest)
{
    if (*(ascii_char_ptr_no_quotes*)dest != NULL)
    {
        free(*(ascii_char_ptr_no_quotes*)dest);
    }
}

/*Codes_SRS_SERIALIZER_99_051:[ EDM_DATE_TIME_OFFSET*/
/*Codes_SRS_SERIALIZER_99_053:[Create_AGENT_DATA_TYPE_from_EDM_DATE_TIME_OFFSET]*/
static AGENT_DATA_TYPES_RESULT MU_C2(ToAGENT_DATA_TYPE_, EDM_DATE_TIME_OFFSET)(AGENT_DATA_TYPE* dest, EDM_DATE_TIME_OFFSET source)
{
    return Create_AGENT_DATA_TYPE_from_EDM_DATE_TIME_OFFSET(dest, source);
}

static AGENT_DATA_TYPES_RESULT MU_C2(FromAGENT_DATA_TYPE_, EDM_DATE_TIME_OFFSET)(const AGENT_DATA_TYPE* agentData, EDM_DATE_TIME_OFFSET* dest)
{
    AGENT_DATA_TYPES_RESULT result;
    if (agentData->type != EDM_DATE_TIME_OFFSET_TYPE)
    {
        result = AGENT_DATA_TYPES_INVALID_ARG;
    }
    else
    {
        *dest = agentData->value.edmDateTimeOffset;
        result = AGENT_DATA_TYPES_OK;
    }
    return result;
}

static void MU_C2(GlobalInitialize_, EDM_DATE_TIME_OFFSET)(void* dest)
{
    memset(dest, 0, sizeof(EDM_DATE_TIME_OFFSET));
}

static void MU_C2(GlobalDeinitialize_, EDM_DATE_TIME_OFFSET)(void* dest)
{
    (void)(dest);
}

/*Codes_SRS_SERIALIZER_99_072:[ EDM_GUID]*/
/*Codes_SRS_SERIALIZER_99_073:[ Create_AGENT_DATA_TYPE_from_EDM_GUID]*/
static AGENT_DATA_TYPES_RESULT MU_C2(ToAGENT_DATA_TYPE_, EDM_GUID)(AGENT_DATA_TYPE* dest, EDM_GUID guid)
{
    return Create_AGENT_DATA_TYPE_from_EDM_GUID(dest, guid);
}

static AGENT_DATA_TYPES_RESULT MU_C2(FromAGENT_DATA_TYPE_, EDM_GUID)(const AGENT_DATA_TYPE* agentData, EDM_GUID* dest)
{
    AGENT_DATA_TYPES_RESULT result;
    if (agentData->type != EDM_GUID_TYPE)
    {
        result = AGENT_DATA_TYPES_INVALID_ARG;
    }
    else
    {
        (void)memcpy(dest->GUID, agentData->value.edmGuid.GUID, 16);
        result = AGENT_DATA_TYPES_OK;
    }
    return result;
}

static void MU_C2(GlobalInitialize_, EDM_GUID)(void* dest)
{
    memset(dest, 0, sizeof(EDM_GUID));
}

static void MU_C2(GlobalDeinitialize_, EDM_GUID)(void* dest)
{
    (void)(dest);
}


/*Codes_SRS_SERIALIZER_99_074:[ EDM_BINARY]*/
/*Codes_SRS_SERIALIZER_99_075:[ Create_AGENT_DATA_TYPE_from_EDM_BINARY]*/
static AGENT_DATA_TYPES_RESULT MU_C2(ToAGENT_DATA_TYPE_, EDM_BINARY)(AGENT_DATA_TYPE* dest, EDM_BINARY edmBinary)
{
    return Create_AGENT_DATA_TYPE_from_EDM_BINARY(dest, edmBinary);
}

static AGENT_DATA_TYPES_RESULT MU_C2(FromAGENT_DATA_TYPE_, EDM_BINARY)(const AGENT_DATA_TYPE* agentData, EDM_BINARY* dest)
{
    AGENT_DATA_TYPES_RESULT result;
    if (agentData->type != EDM_BINARY_TYPE)
    {
        result = AGENT_DATA_TYPES_INVALID_ARG;
    }
    else
    {
        if ((dest->data = (unsigned char *)malloc(agentData->value.edmBinary.size)) == NULL) /*cast because this get included in a C++ file.*/
        {
            result = AGENT_DATA_TYPES_ERROR;
        }
        else
        {
            (void)memcpy(dest->data, agentData->value.edmBinary.data, agentData->value.edmBinary.size);
            dest->size = agentData->value.edmBinary.size;
            result = AGENT_DATA_TYPES_OK;
        }
    }
    return result;
}

static void MU_C2(GlobalInitialize_, EDM_BINARY)(void* dest)
{
    ((EDM_BINARY*)dest)->data = NULL;
    ((EDM_BINARY*)dest)->size = 0;
}

static void MU_C2(GlobalDeinitialize_, EDM_BINARY)(void* dest)
{
    if ((((EDM_BINARY*)dest)->data) != NULL)
    {
        free(((EDM_BINARY*)dest)->data);
    }
}

static void MU_C2(destroyLocalParameter, EDM_BINARY)(EDM_BINARY* value)
{
    if (value != NULL)
    {
        free(value->data);
        value->data = NULL;
        value->size = 0;
    }
}

static void MU_C2(destroyLocalParameter, EDM_BOOLEAN)(EDM_BOOLEAN* value)
{
    (void)value;
}

static void MU_C2(destroyLocalParameter, EDM_BYTE)(EDM_BYTE* value)
{
    (void)value;
}

static void MU_C2(destroyLocalParameter, EDM_DATE)(EDM_DATE* value)
{
    (void)value;
}

static void MU_C2(destroyLocalParameter, EDM_DATE_TIME_OFFSET)(EDM_DATE_TIME_OFFSET* value)
{
    (void)value;
}

static void MU_C2(destroyLocalParameter, EDM_DECIMAL)(EDM_DECIMAL* value)
{
    if (value != NULL)
    {
        STRING_delete(value->value);
        value->value = NULL;
    }
}

static void MU_C2(destroyLocalParameter, EDM_DOUBLE)(EDM_DOUBLE* value)
{
    (void)value;
}

static void MU_C2(destroyLocalParameter, EDM_DURATION)(EDM_DURATION* value)
{
    if (value != NULL)
    {
        free(value->digits);
        value->digits = NULL;
        value->nDigits = 0;
    }
}

static void MU_C2(destroyLocalParameter, EDM_GUID)(EDM_GUID* value)
{
    (void)value;
}

static void MU_C2(destroyLocalParameter, EDM_INT16)(EDM_INT16* value)
{
    (void)value;
}

static void MU_C2(destroyLocalParameter, EDM_INT32)(EDM_INT32* value)
{
    (void)value;
}

static void MU_C2(destroyLocalParameter, EDM_INT64)(EDM_INT64* value)
{
    (void)value;
}

static void MU_C2(destroyLocalParameter, EDM_SBYTE)(EDM_SBYTE* value)
{
    (void)value;
}

static void MU_C2(destroyLocalParameter, EDM_SINGLE)(EDM_SINGLE* value)
{
    (void)value;
}

static void MU_C2(destroyLocalParameter, EDM_STRING)(EDM_STRING* value)
{
    (void)value;
}


static void MU_C2(destroyLocalParameter, EDM_TIME_OF_DAY)(EDM_TIME_OF_DAY* value)
{
    (void)value;
}

static void MU_C2(destroyLocalParameter, int)(int* value)
{
    (void)value;
}

static void MU_C2(destroyLocalParameter, float)(float* value)
{
    (void)value;
}

static void MU_C2(destroyLocalParameter, double)(double* value)
{
    (void)value;
}

static void MU_C2(destroyLocalParameter, long)(long* value)
{
    (void)value;
}

static void MU_C2(destroyLocalParameter, int8_t)(int8_t* value)
{
    (void)value;
}

static void MU_C2(destroyLocalParameter, uint8_t)(uint8_t* value)
{
    (void)value;
}

static void MU_C2(destroyLocalParameter, int16_t)(int16_t* value)
{
    (void)value;
}

static void MU_C2(destroyLocalParameter, int32_t)(int32_t* value)
{
    (void)value;
}

static void MU_C2(destroyLocalParameter, int64_t)(int64_t* value)
{
    (void)value;
}

static void MU_C2(destroyLocalParameter, bool)(bool* value)
{
    (void)value;
}

static void MU_C2(destroyLocalParameter, ascii_char_ptr)(ascii_char_ptr* value)
{
    if (value != NULL)
    {
        free(*value);
    }

}

static void MU_C2(destroyLocalParameter, ascii_char_ptr_no_quotes)(ascii_char_ptr_no_quotes* value)
{
    if (value != NULL)
    {
        free(*value);
    }
}

#ifdef __cplusplus
    }
#endif

#endif /*SERIALIZER_H*/


