// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef AMQPVALUE_TO_STRING_H

#include "azure_uamqp_c/amqpvalue.h"
#include "umock_c/umock_c_prod.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

    MOCKABLE_FUNCTION(, char*, amqpvalue_to_string, AMQP_VALUE, amqp_value);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* AMQPVALUE_TO_STRING_H */
