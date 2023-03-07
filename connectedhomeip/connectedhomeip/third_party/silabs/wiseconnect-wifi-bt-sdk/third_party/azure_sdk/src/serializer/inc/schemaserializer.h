// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef SCHEMASERIALIZER_H
#define SCHEMASERIALIZER_H

#include "schema.h"
#include "azure_c_shared_utility/strings.h"

#ifdef __cplusplus
extern "C" {
#endif

#define SCHEMA_SERIALIZER_RESULT_VALUES    \
    SCHEMA_SERIALIZER_OK,            \
    SCHEMA_SERIALIZER_INVALID_ARG,  \
    SCHEMA_SERIALIZER_ERROR

MU_DEFINE_ENUM_WITHOUT_INVALID(SCHEMA_SERIALIZER_RESULT, SCHEMA_SERIALIZER_RESULT_VALUES)

extern SCHEMA_SERIALIZER_RESULT SchemaSerializer_SerializeCommandMetadata(SCHEMA_MODEL_TYPE_HANDLE modelHandle, STRING_HANDLE schemaText);

#ifdef __cplusplus
}
#endif

#endif /* SCHEMASERIALIZER_H */
