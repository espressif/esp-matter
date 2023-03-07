// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef MESSAGING_H
#define MESSAGING_H

#include "azure_uamqp_c/amqpvalue.h"

#ifdef __cplusplus
#include <cstdint>
extern "C" {
#else
#include <stdint.h>
#endif /* __cplusplus */

#include "umock_c/umock_c_prod.h"

    MOCKABLE_FUNCTION(, AMQP_VALUE, messaging_create_source, const char*, address);
    MOCKABLE_FUNCTION(, AMQP_VALUE, messaging_create_target, const char*, address);

    MOCKABLE_FUNCTION(, AMQP_VALUE, messaging_delivery_received, uint32_t, section_number, uint64_t, section_offset);
    MOCKABLE_FUNCTION(, AMQP_VALUE, messaging_delivery_accepted);
    MOCKABLE_FUNCTION(, AMQP_VALUE, messaging_delivery_rejected, const char*, error_condition, const char*, error_description);
    MOCKABLE_FUNCTION(, AMQP_VALUE, messaging_delivery_released);
    MOCKABLE_FUNCTION(, AMQP_VALUE, messaging_delivery_modified, bool, delivery_failed, bool, undeliverable_here, fields, message_annotations);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* MESSAGING_H */
