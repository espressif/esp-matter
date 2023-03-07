// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "azure_c_shared_utility/xlogging.h"
#include "azure_uamqp_c/amqpvalue.h"
#include "azure_uamqp_c/amqp_definitions.h"

AMQP_VALUE messaging_create_source(const char* address)
{
    AMQP_VALUE result;
    SOURCE_HANDLE source = source_create();

    if (source == NULL)
    {
        LogError("NULL source");
        result = NULL;
    }
    else
    {
        AMQP_VALUE address_value = amqpvalue_create_string(address);
        if (address_value == NULL)
        {
            LogError("Cannot create address AMQP string");
            result = NULL;
        }
        else
        {
            if (source_set_address(source, address_value) != 0)
            {
                LogError("Cannot set address on source");
                result = NULL;
            }
            else
            {
                result = amqpvalue_create_source(source);
                if (result == NULL)
                {
                    LogError("Cannot create source");
                }
                else
                {
                    /* all ok */
                }
            }

            amqpvalue_destroy(address_value);
        }

        source_destroy(source);
    }

    return result;
}

AMQP_VALUE messaging_create_target(const char* address)
{
    AMQP_VALUE result;
    TARGET_HANDLE target = target_create();

    if (target == NULL)
    {
        LogError("NULL target");
        result = NULL;
    }
    else
    {
        AMQP_VALUE address_value = amqpvalue_create_string(address);
        if (address_value == NULL)
        {
            LogError("Cannot create address AMQP string");
            result = NULL;
        }
        else
        {
            if (target_set_address(target, address_value) != 0)
            {
                LogError("Cannot set address on target");
                result = NULL;
            }
            else
            {
                result = amqpvalue_create_target(target);
                if (result == NULL)
                {
                    LogError("Cannot create target");
                }
                else
                {
                    /* all ok */
                }
            }

            amqpvalue_destroy(address_value);
        }

        target_destroy(target);
    }

    return result;
}

AMQP_VALUE messaging_delivery_received(uint32_t section_number, uint64_t section_offset)
{
    AMQP_VALUE result;
    RECEIVED_HANDLE received = received_create(section_number, section_offset);
    if (received == NULL)
    {
        LogError("Cannot create RECEIVED delivery state handle");
        result = NULL;
    }
    else
    {
        result = amqpvalue_create_received(received);
        if (result == NULL)
        {
            LogError("Cannot create RECEIVED delivery state AMQP value");
        }
        else
        {
            /* all ok */
        }

        received_destroy(received);
    }

    return result;
}

AMQP_VALUE messaging_delivery_accepted(void)
{
    AMQP_VALUE result;
    ACCEPTED_HANDLE accepted = accepted_create();
    if (accepted == NULL)
    {
        LogError("Cannot create ACCEPTED delivery state handle");
        result = NULL;
    }
    else
    {
        result = amqpvalue_create_accepted(accepted);
        if (result == NULL)
        {
            LogError("Cannot create ACCEPTED delivery state AMQP value");
        }
        else
        {
            /* all ok */
        }

        accepted_destroy(accepted);
    }

    return result;
}

AMQP_VALUE messaging_delivery_rejected(const char* error_condition, const char* error_description)
{
    AMQP_VALUE result;
    REJECTED_HANDLE rejected = rejected_create();
    if (rejected == NULL)
    {
        LogError("Cannot create REJECTED delivery state handle");
        result = NULL;
    }
    else
    {
        ERROR_HANDLE error_handle = NULL;
        bool error_constructing = false;

        if (error_condition != NULL)
        {
            error_handle = error_create(error_condition);
            if (error_handle == NULL)
            {
                LogError("Cannot create error AMQP value for REJECTED state");
                error_constructing = true;
            }
            else
            {
                if ((error_description != NULL) &&
                    (error_set_description(error_handle, error_description) != 0))
                {
                    LogError("Cannot set error description on error AMQP value for REJECTED state");
                    error_constructing = true;
                }
                else
                {
                    if (rejected_set_error(rejected, error_handle) != 0)
                    {
                        LogError("Cannot set error on REJECTED state handle");
                        error_constructing = true;
                    }
                }

                error_destroy(error_handle);
            }
        }

        if (error_constructing)
        {
            result = NULL;
        }
        else
        {
            result = amqpvalue_create_rejected(rejected);
            if (result == NULL)
            {
                LogError("Cannot create REJECTED delivery state AMQP value");
            }
            else
            {
                /* all ok */
            }
        }

        rejected_destroy(rejected);
    }

    return result;
}

AMQP_VALUE messaging_delivery_released(void)
{
    AMQP_VALUE result;
    RELEASED_HANDLE released = released_create();
    if (released == NULL)
    {
        LogError("Cannot create RELEASED delivery state handle");
        result = NULL;
    }
    else
    {
        result = amqpvalue_create_released(released);
        if (result == NULL)
        {
            LogError("Cannot create RELEASED delivery state AMQP value");
        }
        else
        {
            /* all ok */
        }

        released_destroy(released);
    }

    return result;
}

AMQP_VALUE messaging_delivery_modified(bool delivery_failed, bool undeliverable_here, fields message_annotations)
{
    AMQP_VALUE result;
    MODIFIED_HANDLE modified = modified_create();
    if (modified == NULL)
    {
        LogError("Cannot create MODIFIED delivery state handle");
        result = NULL;
    }
    else
    {
        if (modified_set_delivery_failed(modified, delivery_failed) != 0)
        {
            LogError("Cannot set delivery failed on MODIFIED delivery state");
            result = NULL;
        }
        else if (modified_set_undeliverable_here(modified, undeliverable_here) != 0)
        {
            LogError("Cannot set undeliverable here on MODIFIED delivery state");
            result = NULL;
        }
        else if ((message_annotations != NULL) && (modified_set_message_annotations(modified, message_annotations) != 0))
        {
            LogError("Cannot set message annotations on MODIFIED delivery state");
            result = NULL;
        }
        else
        {
            result = amqpvalue_create_modified(modified);
            if (result == NULL)
            {
                LogError("Cannot create MODIFIED delivery state AMQP value");
            }
            else
            {
                /* all ok */
            }
        }

        modified_destroy(modified);
    }

    return result;
}
