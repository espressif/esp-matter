// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef ANQP_TYPES_H
#define ANQP_TYPES_H

#include <stddef.h>
#include "azure_macro_utils/macro_utils.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define AMQP_TYPE_VALUES \
    AMQP_TYPE_NULL, \
    AMQP_TYPE_BOOL, \
    AMQP_TYPE_UBYTE, \
    AMQP_TYPE_USHORT, \
    AMQP_TYPE_UINT, \
    AMQP_TYPE_ULONG, \
    AMQP_TYPE_BYTE, \
    AMQP_TYPE_SHORT, \
    AMQP_TYPE_INT, \
    AMQP_TYPE_LONG, \
    AMQP_TYPE_FLOAT, \
    AMQP_TYPE_DOUBLE, \
    AMQP_TYPE_CHAR, \
    AMQP_TYPE_TIMESTAMP, \
    AMQP_TYPE_UUID, \
    AMQP_TYPE_BINARY, \
    AMQP_TYPE_STRING, \
    AMQP_TYPE_SYMBOL, \
    AMQP_TYPE_LIST, \
    AMQP_TYPE_MAP, \
    AMQP_TYPE_ARRAY, \
    AMQP_TYPE_DESCRIBED, \
    AMQP_TYPE_COMPOSITE, \
    AMQP_TYPE_UNKNOWN

MU_DEFINE_ENUM(AMQP_TYPE, AMQP_TYPE_VALUES);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* ANQP_TYPES_H */
