// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

/** @file   schemalib.h
 *    @brief    The IoT Hub Serializer APIs allows developers to define models for
*            their devices
 */

#ifndef SCHEMALIB_H
#define SCHEMALIB_H

#include "azure_macro_utils/macro_utils.h"
#include "azure_c_shared_utility/strings.h"
#include "iotdevice.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Codes_SRS_SCHEMALIB__99_001:[ IoTHubSchema_Client shall expose the following API: ... ] */
#define SERIALIZER_RESULT_VALUES                   \
    SERIALIZER_OK,                                 \
    SERIALIZER_INVALID_ARG,                        \
    SERIALIZER_CODEFIRST_INIT_FAILED,              \
    SERIALIZER_SCHEMA_FAILED,                      \
    SERIALIZER_HTTP_API_INIT_FAILED,               \
    SERIALIZER_ALREADY_INIT,                       \
    SERIALIZER_ERROR,                              \
    SERIALIZER_NOT_INITIALIZED,                    \
    SERIALIZER_ALREADY_STARTED,                    \
    SERIALIZER_DEVICE_CREATE_FAILED,               \
    SERIALIZER_GET_MODEL_HANDLE_FAILED,            \
    SERIALIZER_SERVICEBUS_FAILED

/** @brief Enumeration specifying the status of calls to various APIs in this
 * module.
 */
MU_DEFINE_ENUM_WITHOUT_INVALID(SERIALIZER_RESULT, SERIALIZER_RESULT_VALUES);

#define SERIALIZER_CONFIG_VALUES  \
    CommandPollingInterval,     \
    SerializeDelayedBufferMaxSize

/** @brief Enumeration specifying the option to set on the serializer when
 * calling ::serializer_setconfig.
 */
MU_DEFINE_ENUM_WITHOUT_INVALID(SERIALIZER_CONFIG, SERIALIZER_CONFIG_VALUES);

/**
 * @brief   Initializes the library.
 *
 * @param   overrideSchemaNamespace An override schema namespace to use for all
 *                                  models. Optional, can be @c NULL.
 *
 *          If @p schemaNamespace is not @c NULL, its value shall be used
 *          instead of the namespace defined for each model by using the
 *          @c DECLARE_XXX macro.
 *
 * @return  @c SERIALIZER_OK on success and any other error on failure.
 */
extern SERIALIZER_RESULT serializer_init(const char* overrideSchemaNamespace);

/** @brief  Shuts down the IOT library.
 *
 *          The library will track all created devices and upon a call to
 *          ::serializer_deinit it will de-initialize all devices.
 */
extern void serializer_deinit(void);

/**
 * @brief   Set serializer options.
 *
 * @param   which   The option to be set.
 * @param   value   The value to set for the given option.
 *
 * @return  @c SERIALIZER_OK on success and any other error on failure.
 */
extern SERIALIZER_RESULT serializer_setconfig(SERIALIZER_CONFIG which, void* value);

#ifdef __cplusplus
}
#endif

#endif /* SCHEMALIB_H */
