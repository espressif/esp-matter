

// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

// This file is generated. DO NOT EDIT it manually.
// The generator that produces it is located at /uamqp_generator/uamqp_generator.sln

#ifndef AMQP_DEFINITIONS_H
#define AMQP_DEFINITIONS_H

#ifdef __cplusplus
#include <cstdint>
extern "C" {
#else
#include <stdint.h>
#include <stdbool.h>
#endif

#include "azure_uamqp_c/amqpvalue.h"
#include "umock_c/umock_c_prod.h"

#include "azure_uamqp_c/amqp_definitions_role.h"
#include "azure_uamqp_c/amqp_definitions_sender_settle_mode.h"
#include "azure_uamqp_c/amqp_definitions_receiver_settle_mode.h"
#include "azure_uamqp_c/amqp_definitions_handle.h"
#include "azure_uamqp_c/amqp_definitions_seconds.h"
#include "azure_uamqp_c/amqp_definitions_milliseconds.h"
#include "azure_uamqp_c/amqp_definitions_delivery_tag.h"
#include "azure_uamqp_c/amqp_definitions_sequence_no.h"
#include "azure_uamqp_c/amqp_definitions_delivery_number.h"
#include "azure_uamqp_c/amqp_definitions_transfer_number.h"
#include "azure_uamqp_c/amqp_definitions_message_format.h"
#include "azure_uamqp_c/amqp_definitions_ietf_language_tag.h"
#include "azure_uamqp_c/amqp_definitions_fields.h"
#include "azure_uamqp_c/amqp_definitions_error.h"
#include "azure_uamqp_c/amqp_definitions_amqp_error.h"
#include "azure_uamqp_c/amqp_definitions_connection_error.h"
#include "azure_uamqp_c/amqp_definitions_session_error.h"
#include "azure_uamqp_c/amqp_definitions_link_error.h"
#include "azure_uamqp_c/amqp_definitions_open.h"
#include "azure_uamqp_c/amqp_definitions_begin.h"
#include "azure_uamqp_c/amqp_definitions_attach.h"
#include "azure_uamqp_c/amqp_definitions_flow.h"
#include "azure_uamqp_c/amqp_definitions_transfer.h"
#include "azure_uamqp_c/amqp_definitions_disposition.h"
#include "azure_uamqp_c/amqp_definitions_detach.h"
#include "azure_uamqp_c/amqp_definitions_end.h"
#include "azure_uamqp_c/amqp_definitions_close.h"
#include "azure_uamqp_c/amqp_definitions_sasl_code.h"
#include "azure_uamqp_c/amqp_definitions_sasl_mechanisms.h"
#include "azure_uamqp_c/amqp_definitions_sasl_init.h"
#include "azure_uamqp_c/amqp_definitions_sasl_challenge.h"
#include "azure_uamqp_c/amqp_definitions_sasl_response.h"
#include "azure_uamqp_c/amqp_definitions_sasl_outcome.h"
#include "azure_uamqp_c/amqp_definitions_terminus_durability.h"
#include "azure_uamqp_c/amqp_definitions_terminus_expiry_policy.h"
#include "azure_uamqp_c/amqp_definitions_node_properties.h"
#include "azure_uamqp_c/amqp_definitions_filter_set.h"
#include "azure_uamqp_c/amqp_definitions_source.h"
#include "azure_uamqp_c/amqp_definitions_target.h"
#include "azure_uamqp_c/amqp_definitions_annotations.h"
#include "azure_uamqp_c/amqp_definitions_message_id_ulong.h"
#include "azure_uamqp_c/amqp_definitions_message_id_uuid.h"
#include "azure_uamqp_c/amqp_definitions_message_id_binary.h"
#include "azure_uamqp_c/amqp_definitions_message_id_string.h"
#include "azure_uamqp_c/amqp_definitions_address_string.h"
#include "azure_uamqp_c/amqp_definitions_header.h"
#include "azure_uamqp_c/amqp_definitions_delivery_annotations.h"
#include "azure_uamqp_c/amqp_definitions_message_annotations.h"
#include "azure_uamqp_c/amqp_definitions_application_properties.h"
#include "azure_uamqp_c/amqp_definitions_data.h"
#include "azure_uamqp_c/amqp_definitions_amqp_sequence.h"
#include "azure_uamqp_c/amqp_definitions_amqp_value.h"
#include "azure_uamqp_c/amqp_definitions_footer.h"
#include "azure_uamqp_c/amqp_definitions_properties.h"
#include "azure_uamqp_c/amqp_definitions_received.h"
#include "azure_uamqp_c/amqp_definitions_accepted.h"
#include "azure_uamqp_c/amqp_definitions_rejected.h"
#include "azure_uamqp_c/amqp_definitions_released.h"
#include "azure_uamqp_c/amqp_definitions_modified.h"

#ifdef __cplusplus
}
#endif

#endif /* AMQP_DEFINITIONS_H */
