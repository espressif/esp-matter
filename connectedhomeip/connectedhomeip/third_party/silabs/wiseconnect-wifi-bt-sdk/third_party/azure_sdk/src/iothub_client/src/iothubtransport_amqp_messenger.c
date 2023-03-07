// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>
#include <stdbool.h>
#include "azure_c_shared_utility/optimize_size.h"
#include "azure_c_shared_utility/crt_abstractions.h"
#include "azure_c_shared_utility/gballoc.h"
#include "azure_c_shared_utility/agenttime.h"
#include "azure_c_shared_utility/xlogging.h"
#include "azure_c_shared_utility/uniqueid.h"
#include "azure_uamqp_c/link.h"
#include "azure_uamqp_c/messaging.h"
#include "azure_uamqp_c/message_sender.h"
#include "azure_uamqp_c/message_receiver.h"
#include "internal/message_queue.h"
#include "internal/iothub_client_retry_control.h"
#include "internal/iothubtransport_amqp_messenger.h"
#include "iothub_client_options.h"

MU_DEFINE_ENUM_STRINGS_WITHOUT_INVALID(AMQP_MESSENGER_SEND_STATUS, AMQP_MESSENGER_SEND_STATUS_VALUES);
MU_DEFINE_ENUM_STRINGS_WITHOUT_INVALID(AMQP_MESSENGER_SEND_RESULT, AMQP_MESSENGER_SEND_RESULT_VALUES);
MU_DEFINE_ENUM_STRINGS_WITHOUT_INVALID(AMQP_MESSENGER_REASON, AMQP_MESSENGER_REASON_VALUES);
MU_DEFINE_ENUM_STRINGS_WITHOUT_INVALID(AMQP_MESSENGER_DISPOSITION_RESULT, AMQP_MESSENGER_DISPOSITION_RESULT_VALUES);
MU_DEFINE_ENUM_STRINGS_WITHOUT_INVALID(AMQP_MESSENGER_STATE, AMQP_MESSENGER_STATE_VALUES);


#define RESULT_OK 0
#define INDEFINITE_TIME ((time_t)(-1))

// AMQP Link address format: "amqps://<iot hub fqdn>/devices/<device-id>/<suffix>"
#define LINK_ADDRESS_FORMAT                             "amqps://%s/devices/%s/%s"
#define LINK_ADDRESS_MODULE_FORMAT                      "amqps://%s/devices/%s/modules/%s/%s"
#define SEND_LINK_NAME_PREFIX                            "link-snd"
#define MESSAGE_SENDER_MAX_LINK_SIZE                    UINT64_MAX
#define RECEIVE_LINK_NAME_PREFIX                        "link-rcv"
#define MESSAGE_RECEIVER_MAX_LINK_SIZE                  65536
#define DEFAULT_EVENT_SEND_RETRY_LIMIT                  0
#define DEFAULT_EVENT_SEND_TIMEOUT_SECS                 600
#define DEFAULT_MAX_SEND_ERROR_COUNT                    10
#define MAX_MESSAGE_SENDER_STATE_CHANGE_TIMEOUT_SECS    300
#define MAX_MESSAGE_RECEIVER_STATE_CHANGE_TIMEOUT_SECS  300
#define UNIQUE_ID_BUFFER_SIZE                           37

static const char* MESSENGER_SAVED_MQ_OPTIONS = "amqp_message_queue_options";

typedef struct AMQP_MESSENGER_INSTANCE_TAG
{
    AMQP_MESSENGER_CONFIG* config;

    bool receive_messages;
    ON_AMQP_MESSENGER_MESSAGE_RECEIVED on_message_received_callback;
    void* on_message_received_context;

    MESSAGE_QUEUE_HANDLE send_queue;
    AMQP_MESSENGER_STATE state;

    SESSION_HANDLE session_handle;

    LINK_HANDLE sender_link;
    MESSAGE_SENDER_HANDLE message_sender;
    MESSAGE_SENDER_STATE message_sender_current_state;
    MESSAGE_SENDER_STATE message_sender_previous_state;

    LINK_HANDLE receiver_link;
    MESSAGE_RECEIVER_HANDLE message_receiver;
    MESSAGE_RECEIVER_STATE message_receiver_current_state;
    MESSAGE_RECEIVER_STATE message_receiver_previous_state;

    size_t send_error_count;
    size_t max_send_error_count;

    time_t last_message_sender_state_change_time;
    time_t last_message_receiver_state_change_time;
} AMQP_MESSENGER_INSTANCE;

typedef struct MESSAGE_SEND_CONTEXT_TAG
{
    MESSAGE_HANDLE message;
    bool is_destroyed;

    AMQP_MESSENGER_INSTANCE* messenger;

    AMQP_MESSENGER_SEND_COMPLETE_CALLBACK on_send_complete_callback;
    void* user_context;

    PROCESS_MESSAGE_COMPLETED_CALLBACK on_process_message_completed_callback;
} MESSAGE_SEND_CONTEXT;



static MESSAGE_SEND_CONTEXT* create_message_send_context()
{
    MESSAGE_SEND_CONTEXT* result;

    if ((result = (MESSAGE_SEND_CONTEXT*)malloc(sizeof(MESSAGE_SEND_CONTEXT))) == NULL)
    {
        LogError("Failed creating the message send context");
    }
    else
    {
        memset(result, 0, sizeof(MESSAGE_SEND_CONTEXT));
    }

    return result;
}

static bool is_valid_configuration(const AMQP_MESSENGER_CONFIG* config)
{
    bool result;

    if (config == NULL)
    {
        LogError("Invalid configuration (NULL)");
        result = false;
    }
    else if (config->prod_info_cb == NULL ||
        config->device_id == NULL ||
        config->iothub_host_fqdn == NULL ||
        config->receive_link.source_suffix == NULL ||
        config->send_link.target_suffix == NULL)
    {
        LogError("Invalid configuration (prod_info_cb=%p, device_id=%p, iothub_host_fqdn=%p, receive_link (source_suffix=%p), send_link (target_suffix=%p))",
            config->prod_info_cb, config->device_id, config->iothub_host_fqdn,
            config->receive_link.source_suffix, config->send_link.target_suffix);
        result = false;
    }
    else
    {
        result = true;
    }

    return result;
}

static void destroy_link_configuration(AMQP_MESSENGER_LINK_CONFIG* link_config)
{
    if (link_config->target_suffix != NULL)
    {
        free((void*)link_config->target_suffix);
        link_config->target_suffix = NULL;
    }

    if (link_config->source_suffix != NULL)
    {
        free((void*)link_config->source_suffix);
        link_config->source_suffix = NULL;
    }

    if (link_config->attach_properties != NULL)
    {
        Map_Destroy(link_config->attach_properties);
        link_config->attach_properties = NULL;
    }
}

static void destroy_configuration(AMQP_MESSENGER_CONFIG* config)
{
    if (config != NULL)
    {
        if (config->device_id != NULL)
        {
            free((void*)config->device_id);
        }

        if (config->module_id != NULL)
        {
            free((void*)config->module_id);
        }

        if (config->iothub_host_fqdn != NULL)
        {
            free((void*)config->iothub_host_fqdn);
        }

        destroy_link_configuration(&config->send_link);
        destroy_link_configuration(&config->receive_link);

        free(config);
    }
}

static int clone_link_configuration(role link_role, AMQP_MESSENGER_LINK_CONFIG* dst_config, const AMQP_MESSENGER_LINK_CONFIG* src_config)
{
    int result;

    if (link_role == role_sender &&
        mallocAndStrcpy_s(&dst_config->target_suffix, src_config->target_suffix) != 0)
    {
        LogError("Failed copying send_link_target_suffix");
        result = MU_FAILURE;
    }
    else if (link_role == role_receiver &&
        mallocAndStrcpy_s(&dst_config->source_suffix, src_config->source_suffix) != 0)
    {
        LogError("Failed copying receive_link_source_suffix");
        destroy_link_configuration(dst_config);
        result = MU_FAILURE;
    }
    else if (src_config->attach_properties != NULL &&
        (dst_config->attach_properties = Map_Clone(src_config->attach_properties)) == NULL)
    {
        LogError("Failed copying link attach properties");
        destroy_link_configuration(dst_config);
        result = MU_FAILURE;
    }
    else
    {
        dst_config->snd_settle_mode = src_config->snd_settle_mode;
        dst_config->rcv_settle_mode = src_config->rcv_settle_mode;

        result = RESULT_OK;
    }

    return result;
}

static AMQP_MESSENGER_CONFIG* clone_configuration(const AMQP_MESSENGER_CONFIG* config)
{
    AMQP_MESSENGER_CONFIG* result;

    if ((result = (AMQP_MESSENGER_CONFIG*)malloc(sizeof(AMQP_MESSENGER_CONFIG))) == NULL)
    {
        LogError("Failed allocating AMQP_MESSENGER_CONFIG");
    }
    else
    {
        memset(result, 0, sizeof(AMQP_MESSENGER_CONFIG));

        if (mallocAndStrcpy_s(&result->device_id, config->device_id) != 0)
        {
            LogError("Failed copying device_id");
            destroy_configuration(result);
            result = NULL;
        }
        else if ((config->module_id != NULL) && (mallocAndStrcpy_s(&result->module_id, config->module_id) != 0))
        {
            LogError("Failed copying module_id");
            destroy_configuration(result);
            result = NULL;
        }
        else if (mallocAndStrcpy_s(&result->iothub_host_fqdn, config->iothub_host_fqdn) != 0)
        {
            LogError("Failed copying iothub_host_fqdn");
            destroy_configuration(result);
            result = NULL;
        }
        else if (clone_link_configuration(role_sender, &result->send_link, &config->send_link) != RESULT_OK)
        {
            LogError("Failed copying send link configuration");
            destroy_configuration(result);
            result = NULL;
        }
        else if (clone_link_configuration(role_receiver, &result->receive_link, &config->receive_link) != RESULT_OK)
        {
            LogError("Failed copying receive link configuration");
            destroy_configuration(result);
            result = NULL;
        }
        else
        {
            result->prod_info_cb = config->prod_info_cb;
            result->prod_info_ctx = config->prod_info_ctx;
            result->on_state_changed_callback = config->on_state_changed_callback;
            result->on_state_changed_context = config->on_state_changed_context;
            result->on_subscription_changed_callback = config->on_subscription_changed_callback;
            result->on_subscription_changed_context = config->on_subscription_changed_context;
        }
    }

    return result;
}

static void destroy_message_send_context(MESSAGE_SEND_CONTEXT* context)
{
    free(context);
}

static STRING_HANDLE create_link_address(const char* host_fqdn, const char* device_id, const char* module_id, const char* address_suffix)
{
    STRING_HANDLE link_address;

    if ((link_address = STRING_new()) == NULL)
    {
        LogError("failed creating link_address (STRING_new failed)");
    }
    else
    {
        if (module_id != NULL)
        {
            if (STRING_sprintf(link_address, LINK_ADDRESS_MODULE_FORMAT, host_fqdn, device_id, module_id, address_suffix) != RESULT_OK)
            {
                LogError("Failed creating the link_address for a module (STRING_sprintf failed)");
                STRING_delete(link_address);
                link_address = NULL;
            }
        }
        else
        {
            if (STRING_sprintf(link_address, LINK_ADDRESS_FORMAT, host_fqdn, device_id, address_suffix) != RESULT_OK)
            {
                LogError("Failed creating the link_address (STRING_sprintf failed)");
                STRING_delete(link_address);
                link_address = NULL;
            }
        }
    }
    return link_address;
}

static STRING_HANDLE create_link_terminus_name(STRING_HANDLE link_name, const char* suffix)
{
    STRING_HANDLE terminus_name;

    if ((terminus_name = STRING_new()) == NULL)
    {
        LogError("Failed creating the terminus name (STRING_new failed; %s)", suffix);
    }
    else
    {
        const char* link_name_char_ptr = STRING_c_str(link_name);

        if (STRING_sprintf(terminus_name, "%s-%s", link_name_char_ptr, suffix) != RESULT_OK)
        {
            STRING_delete(terminus_name);
            terminus_name = NULL;
            LogError("Failed creating the terminus name (STRING_sprintf failed; %s)", suffix);
        }
    }

    return terminus_name;
}

static STRING_HANDLE create_link_name(role link_role, const char* device_id)
{
    char* unique_id;
    STRING_HANDLE result;

    if ((unique_id = (char*)malloc(sizeof(char) * UNIQUE_ID_BUFFER_SIZE + 1)) == NULL)
    {
        LogError("Failed generating an unique tag (malloc failed)");
        result = NULL;
    }
    else
    {
        memset(unique_id, 0, sizeof(char) * UNIQUE_ID_BUFFER_SIZE + 1);

        if (UniqueId_Generate(unique_id, UNIQUE_ID_BUFFER_SIZE) != UNIQUEID_OK)
        {
            LogError("Failed generating an unique tag (UniqueId_Generate failed)");
            result = NULL;
        }
        else if ((result = STRING_new()) == NULL)
        {
            LogError("Failed generating an unique tag (STRING_new failed)");
        }
        else if (STRING_sprintf(result, "%s-%s-%s", (link_role == role_sender ? SEND_LINK_NAME_PREFIX : RECEIVE_LINK_NAME_PREFIX), device_id, unique_id) != 0)
        {
            LogError("Failed generating an unique tag (STRING_sprintf failed)");
            STRING_delete(result);
            result = NULL;
        }

        free(unique_id);
    }

    return result;
}

static void update_messenger_state(AMQP_MESSENGER_INSTANCE* instance, AMQP_MESSENGER_STATE new_state)
{
    if (new_state != instance->state)
    {
        AMQP_MESSENGER_STATE previous_state = instance->state;
        instance->state = new_state;

        if (instance->config != NULL && instance->config->on_state_changed_callback != NULL)
        {
            instance->config->on_state_changed_callback(instance->config->on_state_changed_context, previous_state, new_state);
        }
    }
}

static int add_link_attach_properties(LINK_HANDLE link, MAP_HANDLE user_defined_properties)
{
    int result;
    fields attach_properties;

    if ((attach_properties = amqpvalue_create_map()) == NULL)
    {
        LogError("Failed to create the map for attach properties.");
        result = MU_FAILURE;
    }
    else
    {
        const char* const* keys;
        const char* const* values;
        size_t count;

        if (Map_GetInternals(user_defined_properties, &keys, &values, &count) != MAP_OK)
        {
            LogError("failed getting user defined properties details.");
            result = MU_FAILURE;
        }
        else
        {
            size_t i;
            result = RESULT_OK;

            for (i = 0; i < count && result == RESULT_OK; i++)
            {
                AMQP_VALUE key;
                AMQP_VALUE value;

                if ((key = amqpvalue_create_symbol(keys[i])) == NULL)
                {
                    LogError("Failed creating AMQP_VALUE For key %s.", keys[i]);
                    result = MU_FAILURE;
                }
                else
                {
                    if ((value = amqpvalue_create_string(values[i])) == NULL)
                    {
                        LogError("Failed creating AMQP_VALUE For key %s value", keys[i]);
                        result = MU_FAILURE;
                    }
                    else
                    {
                        if (amqpvalue_set_map_value(attach_properties, key, value) != 0)
                        {
                            LogError("Failed adding property %s to map", keys[i]);
                            result = MU_FAILURE;
                        }

                        amqpvalue_destroy(value);
                    }

                    amqpvalue_destroy(key);
                }
            }

            if (result == RESULT_OK)
            {
                if (link_set_attach_properties(link, attach_properties) != 0)
                {
                    LogError("Failed attaching properties to link");
                    result = MU_FAILURE;
                }
                else
                {
                    result = RESULT_OK;
                }
            }
        }

        amqpvalue_destroy(attach_properties);
    }

    return result;
}

static int create_link_terminus(role link_role, STRING_HANDLE link_name, STRING_HANDLE link_address, AMQP_VALUE* source, AMQP_VALUE* target)
{
    int result;
    STRING_HANDLE terminus_name;
    const char* source_name;
    const char* target_name;

    if (link_role == role_sender)
    {
        if ((terminus_name = create_link_terminus_name(link_name, "source")) == NULL)
        {
            LogError("Failed creating terminus name");
            source_name = NULL;
            target_name = NULL;
        }
        else
        {
            // Codes_SRS_IOTHUBTRANSPORT_AMQP_MESSENGER_09_077: [The AMQP link source shall be defined as "<link name>-source"]
            source_name = STRING_c_str(terminus_name);
            // Codes_SRS_IOTHUBTRANSPORT_AMQP_MESSENGER_09_094: [The AMQP link source shall be defined as <link address>]
            target_name = STRING_c_str(link_address);
        }
    }
    else
    {
        if ((terminus_name = create_link_terminus_name(link_name, "target")) == NULL)
        {
            LogError("Failed creating terminus name");
            source_name = NULL;
            target_name = NULL;
        }
        else
        {
            // Codes_SRS_IOTHUBTRANSPORT_AMQP_MESSENGER_09_078: [The AMQP link target shall be defined as <link address>]
            source_name = STRING_c_str(link_address);
            // Codes_SRS_IOTHUBTRANSPORT_AMQP_MESSENGER_09_095: [The AMQP link target shall be defined as "<link name>-target"]
            target_name = STRING_c_str(terminus_name);
        }
    }

    if (source_name == NULL || target_name == NULL)
    {
        LogError("Failed creating link source and/or target name (source=%p, target=%p)", source_name, target_name);
        result = MU_FAILURE;
    }
    else
    {
        if ((*source = messaging_create_source(source_name)) == NULL)
        {
            LogError("Failed creating link source");
            result = MU_FAILURE;
        }
        else
        {
            if ((*target = messaging_create_target(target_name)) == NULL)
            {
                LogError("Failed creating link target");
                amqpvalue_destroy(*source);
                *source = NULL;
                result = MU_FAILURE;
            }
            else
            {
                result = RESULT_OK;
            }
        }
    }


    STRING_delete(terminus_name);

    return result;
}

static LINK_HANDLE create_link(role link_role, SESSION_HANDLE session_handle, AMQP_MESSENGER_LINK_CONFIG* link_config, const char* iothub_host_fqdn, const char* device_id, const char* module_id)
{
    LINK_HANDLE result = NULL;
    STRING_HANDLE link_address;

    // Codes_SRS_IOTHUBTRANSPORT_AMQP_MESSENGER_09_075: [The AMQP link address shall be defined as "amqps://<`iothub_host_fqdn`>/devices/<`device_id`>/<`instance-config->send_link.source_suffix`>"]
    // Codes_SRS_IOTHUBTRANSPORT_AMQP_MESSENGER_09_092: [The AMQP link address shall be defined as "amqps://<`iothub_host_fqdn`>/devices/<`device_id`>/<`instance-config->receive_link.target_suffix`>"]
    if ((link_address = create_link_address(iothub_host_fqdn, device_id, module_id, (link_role == role_sender ? link_config->target_suffix : link_config->source_suffix))) == NULL)
    {
        LogError("Failed creating the message sender (failed creating the 'link_address')");
        result = NULL;
    }
    else
    {
        STRING_HANDLE link_name;

        // Codes_SRS_IOTHUBTRANSPORT_AMQP_MESSENGER_09_076: [The AMQP link name shall be defined as "link-snd-<`device_id`>-<locally generated UUID>"]
        // Codes_SRS_IOTHUBTRANSPORT_AMQP_MESSENGER_09_093: [The AMQP link name shall be defined as "link-rcv-<`device_id`>-<locally generated UUID>"]
        if ((link_name = create_link_name(link_role, device_id)) == NULL)
        {
            LogError("Failed creating the link name");
            result = NULL;
        }
        else
        {
            AMQP_VALUE source = NULL;
            AMQP_VALUE target = NULL;

            if (create_link_terminus(link_role, link_name, link_address, &source, &target) == RESULT_OK)
            {
                if ((result = link_create(session_handle, STRING_c_str(link_name), link_role, source, target)) == NULL)
                {
                    // Codes_SRS_IOTHUBTRANSPORT_AMQP_MESSENGER_09_079: [If the link fails to be created, amqp_messenger_do_work() shall change the state to AMQP_MESSENGER_STATE_ERROR]
                    // Codes_SRS_IOTHUBTRANSPORT_AMQP_MESSENGER_09_096: [If the link fails to be created, amqp_messenger_do_work() shall change the state to AMQP_MESSENGER_STATE_ERROR]
                    LogError("Failed creating the AMQP link");
                }
                else
                {
                    // Codes_SRS_IOTHUBTRANSPORT_AMQP_MESSENGER_09_082: [The AMQP link maximum message size shall be set to UINT64_MAX using link_set_max_message_size()]
                    // Codes_SRS_IOTHUBTRANSPORT_AMQP_MESSENGER_09_099: [The AMQP link maximum message size shall be set to UINT64_MAX using link_set_max_message_size()]
                    if (link_set_max_message_size(result, MESSAGE_SENDER_MAX_LINK_SIZE) != RESULT_OK)
                    {
                        // Codes_SRS_IOTHUBTRANSPORT_AMQP_MESSENGER_09_083: [If link_set_max_message_size() fails, it shall be logged and ignored.]
                        // Codes_SRS_IOTHUBTRANSPORT_AMQP_MESSENGER_09_100: [If link_set_max_message_size() fails, it shall be logged and ignored.]
                        LogError("Failed setting link max message size.");
                    }

                    // Codes_SRS_IOTHUBTRANSPORT_AMQP_MESSENGER_09_080: [The AMQP link shall have its ATTACH properties set using `instance->config->send_link.attach_properties`]
                    // Codes_SRS_IOTHUBTRANSPORT_AMQP_MESSENGER_09_097: [The AMQP link shall have its ATTACH properties set using `instance->config->receive_link.attach_properties`]
                    if (link_config->attach_properties != NULL &&
                        add_link_attach_properties(result, link_config->attach_properties) != RESULT_OK)
                    {
                        // Codes_SRS_IOTHUBTRANSPORT_AMQP_MESSENGER_09_081: [If the AMQP link attach properties fail to be set, amqp_messenger_do_work() shall change the state to AMQP_MESSENGER_STATE_ERROR]
                        // Codes_SRS_IOTHUBTRANSPORT_AMQP_MESSENGER_09_098: [If the AMQP link attach properties fail to be set, amqp_messenger_do_work() shall change the state to AMQP_MESSENGER_STATE_ERROR]
                        LogError("Failed setting link attach properties");
                        link_destroy(result);
                        result = NULL;
                    }
                }

                amqpvalue_destroy(source);
                amqpvalue_destroy(target);
            }

            STRING_delete(link_name);
        }

        STRING_delete(link_address);
    }

    return result;
}

static void destroy_message_sender(AMQP_MESSENGER_INSTANCE* instance)
{
    if (instance->message_sender != NULL)
    {
        // Codes_SRS_IOTHUBTRANSPORT_AMQP_MESSENGER_09_090: [`instance->message_sender` shall be destroyed using messagesender_destroy()]
        messagesender_destroy(instance->message_sender);
        instance->message_sender = NULL;
    }

    instance->message_sender_current_state = MESSAGE_SENDER_STATE_IDLE;
    instance->message_sender_previous_state = MESSAGE_SENDER_STATE_IDLE;
    instance->last_message_sender_state_change_time = INDEFINITE_TIME;

    if (instance->sender_link != NULL)
    {
        // Codes_SRS_IOTHUBTRANSPORT_AMQP_MESSENGER_09_091: [`instance->sender_link` shall be destroyed using link_destroy()]
        link_destroy(instance->sender_link);
        instance->sender_link = NULL;
    }
}

static void on_message_sender_state_changed_callback(void* context, MESSAGE_SENDER_STATE new_state, MESSAGE_SENDER_STATE previous_state)
{
    if (context == NULL)
    {
        LogError("on_message_sender_state_changed_callback was invoked with a NULL context; although unexpected, this failure will be ignored");
    }
    else if (new_state != previous_state)
    {
        AMQP_MESSENGER_INSTANCE* instance = (AMQP_MESSENGER_INSTANCE*)context;

        // Codes_SRS_IOTHUBTRANSPORT_AMQP_MESSENGER_09_088: [`new_state`, `previous_state` shall be saved into `instance->message_sender_previous_state` and `instance->message_sender_current_state`]
        instance->message_sender_current_state = new_state;
        instance->message_sender_previous_state = previous_state;
        // Codes_SRS_IOTHUBTRANSPORT_AMQP_MESSENGER_09_089: [`instance->last_message_sender_state_change_time` shall be set using get_time()]
        instance->last_message_sender_state_change_time = get_time(NULL);
    }
}

static int create_message_sender(AMQP_MESSENGER_INSTANCE* instance)
{
    int result;

    if ((instance->sender_link = create_link(role_sender,
        instance->session_handle, &instance->config->send_link, instance->config->iothub_host_fqdn, instance->config->device_id, instance->config->module_id)) == NULL)
    {
        LogError("Failed creating the message sender link");
        result = MU_FAILURE;
    }
    // Codes_SRS_IOTHUBTRANSPORT_AMQP_MESSENGER_09_084: [`instance->message_sender` shall be created using messagesender_create(), passing the `instance->sender_link` and `on_message_sender_state_changed_callback`]
    else if ((instance->message_sender = messagesender_create(instance->sender_link, on_message_sender_state_changed_callback, (void*)instance)) == NULL)
    {
        // Codes_SRS_IOTHUBTRANSPORT_AMQP_MESSENGER_09_085: [If messagesender_create() fails, amqp_messenger_do_work() shall fail and return]
        LogError("Failed creating the message sender (messagesender_create failed)");
        destroy_message_sender(instance);
        result = MU_FAILURE;
    }
    else
    {
        // Codes_SRS_IOTHUBTRANSPORT_AMQP_MESSENGER_09_086: [`instance->message_sender` shall be opened using messagesender_open()]
        if (messagesender_open(instance->message_sender) != RESULT_OK)
        {
            // Codes_SRS_IOTHUBTRANSPORT_AMQP_MESSENGER_09_087: [If messagesender_open() fails, amqp_messenger_do_work() shall fail and return]
            LogError("Failed opening the AMQP message sender.");
            destroy_message_sender(instance);
            result = MU_FAILURE;
        }
        else
        {
            result = RESULT_OK;
        }
    }

    return result;
}

static void destroy_message_receiver(AMQP_MESSENGER_INSTANCE* instance)
{
    if (instance->message_receiver != NULL)
    {
        // Codes_SRS_IOTHUBTRANSPORT_AMQP_MESSENGER_09_113: [`instance->message_receiver` shall be closed using messagereceiver_close()]
        if (messagereceiver_close(instance->message_receiver) != RESULT_OK)
        {
            // Codes_SRS_IOTHUBTRANSPORT_AMQP_MESSENGER_09_114: [If messagereceiver_close() fails, it shall be logged and ignored]
            LogError("Failed closing the AMQP message receiver (this failure will be ignored).");
        }

        // Codes_SRS_IOTHUBTRANSPORT_AMQP_MESSENGER_09_115: [`instance->message_receiver` shall be destroyed using messagereceiver_destroy()]
        messagereceiver_destroy(instance->message_receiver);

        instance->message_receiver = NULL;
    }

    instance->message_receiver_current_state = MESSAGE_RECEIVER_STATE_IDLE;
    instance->message_receiver_previous_state = MESSAGE_RECEIVER_STATE_IDLE;
    instance->last_message_receiver_state_change_time = INDEFINITE_TIME;

    if (instance->receiver_link != NULL)
    {
        // Codes_SRS_IOTHUBTRANSPORT_AMQP_MESSENGER_09_116: [`instance->receiver_link` shall be destroyed using link_destroy()]
        link_destroy(instance->receiver_link);
        instance->receiver_link = NULL;
    }
}

static void on_message_receiver_state_changed_callback(const void* context, MESSAGE_RECEIVER_STATE new_state, MESSAGE_RECEIVER_STATE previous_state)
{
    if (context == NULL)
    {
        LogError("on_message_receiver_state_changed_callback was invoked with a NULL context; although unexpected, this failure will be ignored");
    }
    else
    {
        if (new_state != previous_state)
        {
            AMQP_MESSENGER_INSTANCE* instance = (AMQP_MESSENGER_INSTANCE*)context;

            // Codes_SRS_IOTHUBTRANSPORT_AMQP_MESSENGER_09_105: [`new_state`, `previous_state` shall be saved into `instance->message_receiver_previous_state` and `instance->message_receiver_current_state`]
            instance->message_receiver_current_state = new_state;
            instance->message_receiver_previous_state = previous_state;
            // Codes_SRS_IOTHUBTRANSPORT_AMQP_MESSENGER_09_106: [`instance->last_message_receiver_state_change_time` shall be set using get_time()]
            instance->last_message_receiver_state_change_time = get_time(NULL);

            if (new_state == MESSAGE_RECEIVER_STATE_OPEN)
            {
                if (instance->config->on_subscription_changed_callback != NULL)
                {
                    instance->config->on_subscription_changed_callback(instance->config->on_subscription_changed_context, true);
                }
            }
            else if (previous_state == MESSAGE_RECEIVER_STATE_OPEN && new_state != MESSAGE_RECEIVER_STATE_OPEN)
            {
                if (instance->config->on_subscription_changed_callback != NULL)
                {
                    instance->config->on_subscription_changed_callback(instance->config->on_subscription_changed_context, false);
                }
            }
        }
    }
}

static AMQP_MESSENGER_MESSAGE_DISPOSITION_INFO* create_message_disposition_info(AMQP_MESSENGER_INSTANCE* messenger)
{
    AMQP_MESSENGER_MESSAGE_DISPOSITION_INFO* result;

    if ((result = (AMQP_MESSENGER_MESSAGE_DISPOSITION_INFO*)malloc(sizeof(AMQP_MESSENGER_MESSAGE_DISPOSITION_INFO))) == NULL)
    {
        LogError("Failed creating AMQP_MESSENGER_MESSAGE_DISPOSITION_INFO container (malloc failed)");
        result = NULL;
    }
    else
    {
        delivery_number message_id;

        if (messagereceiver_get_received_message_id(messenger->message_receiver, &message_id) != RESULT_OK)
        {
            LogError("Failed creating AMQP_MESSENGER_MESSAGE_DISPOSITION_INFO container (messagereceiver_get_received_message_id failed)");
            free(result);
            result = NULL;
        }
        else
        {
            const char* link_name;

            if (messagereceiver_get_link_name(messenger->message_receiver, &link_name) != RESULT_OK)
            {
                LogError("Failed creating AMQP_MESSENGER_MESSAGE_DISPOSITION_INFO container (messagereceiver_get_link_name failed)");
                free(result);
                result = NULL;
            }
            else if (mallocAndStrcpy_s(&result->source, link_name) != RESULT_OK)
            {
                LogError("Failed creating AMQP_MESSENGER_MESSAGE_DISPOSITION_INFO container (failed copying link name)");
                free(result);
                result = NULL;
            }
            else
            {
                result->message_id = message_id;
            }
        }
    }

    return result;
}

static void destroy_message_disposition_info(AMQP_MESSENGER_MESSAGE_DISPOSITION_INFO* disposition_info)
{
    free(disposition_info->source);
    free(disposition_info);
}

static AMQP_VALUE create_uamqp_disposition_result_from(AMQP_MESSENGER_DISPOSITION_RESULT disposition_result)
{
    AMQP_VALUE uamqp_disposition_result;

    if (disposition_result == AMQP_MESSENGER_DISPOSITION_RESULT_NONE)
    {
        uamqp_disposition_result = NULL; // intentionally not sending an answer.
    }
    else if (disposition_result == AMQP_MESSENGER_DISPOSITION_RESULT_ACCEPTED)
    {
        uamqp_disposition_result = messaging_delivery_accepted();
    }
    else if (disposition_result == AMQP_MESSENGER_DISPOSITION_RESULT_RELEASED)
    {
        uamqp_disposition_result = messaging_delivery_released();
    }
    else // id est, if (disposition_result == AMQP_MESSENGER_DISPOSITION_RESULT_REJECTED)
    {
        uamqp_disposition_result = messaging_delivery_rejected("Rejected by application", "Rejected by application");
    }

    return uamqp_disposition_result;
}

static AMQP_VALUE on_message_received_internal_callback(const void* context, MESSAGE_HANDLE message)
{
    AMQP_VALUE result;
    AMQP_MESSENGER_INSTANCE* instance = (AMQP_MESSENGER_INSTANCE*)context;
    AMQP_MESSENGER_MESSAGE_DISPOSITION_INFO* message_disposition_info;

    // Codes_SRS_IOTHUBTRANSPORT_AMQP_MESSENGER_09_107: [A AMQP_MESSENGER_MESSAGE_DISPOSITION_INFO instance shall be created containing the source link name and message delivery ID]
    if ((message_disposition_info = create_message_disposition_info(instance)) == NULL)
    {
        // Codes_SRS_IOTHUBTRANSPORT_AMQP_MESSENGER_09_108: [If the AMQP_MESSENGER_MESSAGE_DISPOSITION_INFO instance fails to be created, on_message_received_internal_callback shall return the result of messaging_delivery_released()]
        LogError("on_message_received_internal_callback failed (failed creating AMQP_MESSENGER_MESSAGE_DISPOSITION_INFO).");
        result = messaging_delivery_released();
    }
    else
    {
        // Codes_SRS_IOTHUBTRANSPORT_AMQP_MESSENGER_09_109: [`instance->on_message_received_callback` shall be invoked passing the `message` and AMQP_MESSENGER_MESSAGE_DISPOSITION_INFO instance]
        AMQP_MESSENGER_DISPOSITION_RESULT disposition_result = instance->on_message_received_callback(message, message_disposition_info, instance->on_message_received_context);

        // Codes_SRS_IOTHUBTRANSPORT_AMQP_MESSENGER_09_110: [If `instance->on_message_received_callback` returns AMQP_MESSENGER_DISPOSITION_RESULT_ACCEPTED, on_message_received_internal_callback shall return the result of messaging_delivery_accepted()]
        // Codes_SRS_IOTHUBTRANSPORT_AMQP_MESSENGER_09_111: [If `instance->on_message_received_callback` returns AMQP_MESSENGER_DISPOSITION_RESULT_RELEASED, on_message_received_internal_callback shall return the result of messaging_delivery_released()]
        // Codes_SRS_IOTHUBTRANSPORT_AMQP_MESSENGER_09_112: [If `instance->on_message_received_callback` returns AMQP_MESSENGER_DISPOSITION_RESULT_REJECTED, on_message_received_internal_callback shall return the result of messaging_delivery_rejected()]
        result = create_uamqp_disposition_result_from(disposition_result);
    }

    return result;
}

static int create_message_receiver(AMQP_MESSENGER_INSTANCE* instance)
{
    int result;

    if ((instance->receiver_link = create_link(role_receiver,
        instance->session_handle, &instance->config->receive_link, instance->config->iothub_host_fqdn, instance->config->device_id, instance->config->module_id)) == NULL)
    {
        LogError("Failed creating the message receiver link");
        result = MU_FAILURE;
    }
    // Codes_SRS_IOTHUBTRANSPORT_AMQP_MESSENGER_09_101: [`instance->message_receiver` shall be created using messagereceiver_create(), passing the `instance->receiver_link` and `on_message_receiver_state_changed_callback`]
    else if ((instance->message_receiver = messagereceiver_create(instance->receiver_link, on_message_receiver_state_changed_callback, (void*)instance)) == NULL)
    {
        // Codes_SRS_IOTHUBTRANSPORT_AMQP_MESSENGER_09_102: [If messagereceiver_create() fails, amqp_messenger_do_work() shall fail and return]
        LogError("Failed creating the message receiver (messagereceiver_create failed)");
        destroy_message_receiver(instance);
        result = MU_FAILURE;
    }
    // Codes_SRS_IOTHUBTRANSPORT_AMQP_MESSENGER_09_103: [`instance->message_receiver` shall be opened using messagereceiver_open() passing `on_message_received_internal_callback`]
    else if (messagereceiver_open(instance->message_receiver, on_message_received_internal_callback, (void*)instance) != RESULT_OK)
    {
        // Codes_SRS_IOTHUBTRANSPORT_AMQP_MESSENGER_09_104: [If messagereceiver_open() fails, amqp_messenger_do_work() shall fail and return]
        LogError("Failed opening the AMQP message receiver.");
        destroy_message_receiver(instance);
        result = MU_FAILURE;
    }
    else
    {
        result = RESULT_OK;
    }

    return result;
}

static void on_send_complete_callback(void* context, MESSAGE_SEND_RESULT send_result, AMQP_VALUE delivery_state)
{
    (void)delivery_state;
    if (context != NULL)
    {
        MESSAGE_QUEUE_RESULT mq_result;
        MESSAGE_SEND_CONTEXT* msg_ctx = (MESSAGE_SEND_CONTEXT*)context;

        if (send_result == MESSAGE_SEND_OK)
        {
            // Codes_SRS_IOTHUBTRANSPORT_AMQP_MESSENGER_09_121: [If no failure occurs, `context->on_process_message_completed_callback` shall be invoked with result MESSAGE_QUEUE_SUCCESS]
            mq_result = MESSAGE_QUEUE_SUCCESS;
        }
        else
        {
            // Codes_SRS_IOTHUBTRANSPORT_AMQP_MESSENGER_09_122: [If a failure occurred, `context->on_process_message_completed_callback` shall be invoked with result MESSAGE_QUEUE_ERROR]
            mq_result = MESSAGE_QUEUE_ERROR;
        }

        msg_ctx->on_process_message_completed_callback(msg_ctx->messenger->send_queue, (MQ_MESSAGE_HANDLE)msg_ctx->message, mq_result, NULL);
    }
}

static void on_process_message_callback(MESSAGE_QUEUE_HANDLE message_queue, MQ_MESSAGE_HANDLE message, PROCESS_MESSAGE_COMPLETED_CALLBACK on_process_message_completed_callback, void* context)
{
    // Codes_SRS_IOTHUBTRANSPORT_AMQP_MESSENGER_09_117: [If any argument is NULL, `on_process_message_callback` shall return immediatelly]
    if (message_queue == NULL || message == NULL || on_process_message_completed_callback == NULL || context == NULL)
    {
        LogError("Invalid argument (message_queue=%p, message=%p, on_process_message_completed_callback=%p, context=%p)", message_queue, message, on_process_message_completed_callback, context);
    }
    else
    {
        MESSAGE_SEND_CONTEXT* message_context = (MESSAGE_SEND_CONTEXT*)context;
        message_context->on_process_message_completed_callback = on_process_message_completed_callback;

        // Codes_SRS_IOTHUBTRANSPORT_AMQP_MESSENGER_09_118: [The MESSAGE_HANDLE shall be submitted for sending using messagesender_send(), passing `on_send_complete_callback`]
        if (messagesender_send_async(message_context->messenger->message_sender, (MESSAGE_HANDLE)message, on_send_complete_callback, context, 0) == NULL)
        {
            // Codes_SRS_IOTHUBTRANSPORT_AMQP_MESSENGER_09_119: [If messagesender_send() fails, `on_process_message_completed_callback` shall be invoked with result MESSAGE_QUEUE_ERROR]
            LogError("Failed sending AMQP message");
            on_process_message_completed_callback(message_queue, message, MESSAGE_QUEUE_ERROR, NULL);
        }

        // Codes_SRS_IOTHUBTRANSPORT_AMQP_MESSENGER_09_120: [The MESSAGE_HANDLE shall be destroyed using message_destroy() and marked as destroyed in the context provided]
        message_destroy((MESSAGE_HANDLE)message);
        message_context->is_destroyed = true;
    }
}

static void on_message_processing_completed_callback(MQ_MESSAGE_HANDLE message, MESSAGE_QUEUE_RESULT result, USER_DEFINED_REASON reason, void* message_context)
{
    (void)reason;

    // Codes_SRS_IOTHUBTRANSPORT_AMQP_MESSENGER_09_020: [If `messenger_context` is NULL, `on_message_processing_completed_callback` shall return immediately]
    if (message_context == NULL)
    {
        LogError("on_message_processing_completed_callback invoked with NULL context");
    }
    else
    {
        MESSAGE_SEND_CONTEXT* msg_ctx = (MESSAGE_SEND_CONTEXT*)message_context;
        AMQP_MESSENGER_SEND_RESULT messenger_send_result;
        AMQP_MESSENGER_REASON messenger_send_reason;

        // Codes_SRS_IOTHUBTRANSPORT_AMQP_MESSENGER_09_021: [If `result` is MESSAGE_QUEUE_SUCCESS, `send_ctx->on_send_complete_callback` shall be invoked with AMQP_MESSENGER_SEND_RESULT_SUCCESS and AMQP_MESSENGER_REASON_NONE]
        if (result == MESSAGE_QUEUE_SUCCESS)
        {
            messenger_send_result = AMQP_MESSENGER_SEND_RESULT_SUCCESS;
            messenger_send_reason = AMQP_MESSENGER_REASON_NONE;

            // Codes_SRS_IOTHUBTRANSPORT_AMQP_MESSENGER_09_025: [If `result` is MESSAGE_QUEUE_SUCCESS or MESSAGE_QUEUE_CANCELLED, `instance->send_error_count` shall be set to 0]
            msg_ctx->messenger->send_error_count = 0;
        }
        // Codes_SRS_IOTHUBTRANSPORT_AMQP_MESSENGER_09_022: [If `result` is MESSAGE_QUEUE_TIMEOUT, `send_ctx->on_send_complete_callback` shall be invoked with AMQP_MESSENGER_SEND_RESULT_ERROR and AMQP_MESSENGER_REASON_TIMEOUT]
        else if (result == MESSAGE_QUEUE_TIMEOUT)
        {
            messenger_send_result = AMQP_MESSENGER_SEND_RESULT_ERROR;
            messenger_send_reason = AMQP_MESSENGER_REASON_TIMEOUT;

            // Codes_SRS_IOTHUBTRANSPORT_AMQP_MESSENGER_09_026: [Otherwise `instance->send_error_count` shall be incremented by 1]
            msg_ctx->messenger->send_error_count++;
        }
        // Codes_SRS_IOTHUBTRANSPORT_AMQP_MESSENGER_09_023: [If `result` is MESSAGE_QUEUE_CANCELLED and the messenger is STOPPED, `send_ctx->on_send_complete_callback` shall be invoked with AMQP_MESSENGER_SEND_RESULT_CANCELLED and AMQP_MESSENGER_REASON_MESSENGER_DESTROYED]
        else if (result == MESSAGE_QUEUE_CANCELLED && msg_ctx->messenger->state == AMQP_MESSENGER_STATE_STOPPED)
        {
            messenger_send_result = AMQP_MESSENGER_SEND_RESULT_CANCELLED;
            messenger_send_reason = AMQP_MESSENGER_REASON_MESSENGER_DESTROYED;

            // Codes_SRS_IOTHUBTRANSPORT_AMQP_MESSENGER_09_025: [If `result` is MESSAGE_QUEUE_SUCCESS or MESSAGE_QUEUE_CANCELLED, `instance->send_error_count` shall be set to 0]
            msg_ctx->messenger->send_error_count = 0;
        }
        // Codes_SRS_IOTHUBTRANSPORT_AMQP_MESSENGER_09_024: [Otherwise `send_ctx->on_send_complete_callback` shall be invoked with AMQP_MESSENGER_SEND_RESULT_ERROR and AMQP_MESSENGER_REASON_FAIL_SENDING]
        else
        {
            messenger_send_result = AMQP_MESSENGER_SEND_RESULT_ERROR;
            messenger_send_reason = AMQP_MESSENGER_REASON_FAIL_SENDING;

            // Codes_SRS_IOTHUBTRANSPORT_AMQP_MESSENGER_09_026: [Otherwise `instance->send_error_count` shall be incremented by 1]
            msg_ctx->messenger->send_error_count++;
        }

        if (msg_ctx->on_send_complete_callback != NULL)
        {
            msg_ctx->on_send_complete_callback(messenger_send_result, messenger_send_reason, msg_ctx->user_context);
        }

        // Codes_SRS_IOTHUBTRANSPORT_AMQP_MESSENGER_09_027: [If `message` has not been destroyed, it shall be destroyed using message_destroy()]
        if (!msg_ctx->is_destroyed)
        {
            message_destroy((MESSAGE_HANDLE)message);
        }

        // Codes_SRS_IOTHUBTRANSPORT_AMQP_MESSENGER_09_028: [`send_ctx` shall be destroyed]
        destroy_message_send_context(msg_ctx);
    }
}


// ---------- Set/Retrieve Options Helpers ----------//

static void* amqp_messenger_clone_option(const char* name, const void* value)
{
    void* result;

    if (name == NULL || value == NULL)
    {
        LogError("invalid argument (name=%p, value=%p)", name, value);
        result = NULL;
    }
    else
    {
        if (strcmp(MESSENGER_SAVED_MQ_OPTIONS, name) == 0)
        {
            if ((result = (void*)OptionHandler_Clone((OPTIONHANDLER_HANDLE)value)) == NULL)
            {
                LogError("failed cloning option '%s'", name);
            }
        }
        else
        {
            LogError("Failed to clone messenger option (option with name '%s' is not suppported)", name);
            result = NULL;
        }
    }

    return result;
}

static void amqp_messenger_destroy_option(const char* name, const void* value)
{
    if (name == NULL || value == NULL)
    {
        LogError("invalid argument (name=%p, value=%p)", name, value);
    }
    else if (strcmp(MESSENGER_SAVED_MQ_OPTIONS, name) == 0)
    {
        OptionHandler_Destroy((OPTIONHANDLER_HANDLE)value);
    }
    else
    {
        LogError("invalid argument (option '%s' is not suppported)", name);
    }
}


// Public API:

int amqp_messenger_subscribe_for_messages(AMQP_MESSENGER_HANDLE messenger_handle, ON_AMQP_MESSENGER_MESSAGE_RECEIVED on_message_received_callback, void* context)
{
    int result;

    // Codes_SRS_IOTHUBTRANSPORT_AMQP_MESSENGER_09_035: [If `messenger_handle` or `on_message_received_callback` are NULL, amqp_messenger_subscribe_for_messages() shall fail and return non-zero value]
    if (messenger_handle == NULL || on_message_received_callback == NULL || context == NULL)
    {
        LogError("Invalid argument (messenger_handle=%p, on_message_received_callback=%p, context=%p)", messenger_handle, on_message_received_callback, context);
        result = MU_FAILURE;
    }
    else
    {
        AMQP_MESSENGER_INSTANCE* instance = (AMQP_MESSENGER_INSTANCE*)messenger_handle;

        // Codes_SRS_IOTHUBTRANSPORT_AMQP_MESSENGER_09_036: [`on_message_received_callback` and `context` shall be saved on `instance->on_message_received_callback`]
        // Codes_SRS_IOTHUBTRANSPORT_AMQP_MESSENGER_09_037: [amqp_messenger_subscribe_for_messages() shall set `instance->receive_messages` to true]
        instance->on_message_received_callback = on_message_received_callback;
        instance->on_message_received_context = context;
        instance->receive_messages = true;

        // Codes_SRS_IOTHUBTRANSPORT_AMQP_MESSENGER_09_038: [If no failures occurr, amqp_messenger_subscribe_for_messages() shall return 0]
        result = RESULT_OK;
    }

    return result;
}

int amqp_messenger_unsubscribe_for_messages(AMQP_MESSENGER_HANDLE messenger_handle)
{
    int result;

    // Codes_SRS_IOTHUBTRANSPORT_AMQP_MESSENGER_09_039: [If `messenger_handle` is NULL, amqp_messenger_unsubscribe_for_messages() shall fail and return non-zero value]
    if (messenger_handle == NULL)
    {
        LogError("Invalid argument (messenger_handle is NULL)");
        result = MU_FAILURE;
    }
    else
    {
        AMQP_MESSENGER_INSTANCE* instance = (AMQP_MESSENGER_INSTANCE*)messenger_handle;

        // Codes_SRS_IOTHUBTRANSPORT_AMQP_MESSENGER_09_040: [`instance->receive_messages` shall be saved to false]
        // Codes_SRS_IOTHUBTRANSPORT_AMQP_MESSENGER_09_041: [`instance->on_message_received_callback` and `instance->on_message_received_context` shall be set to NULL]
        instance->receive_messages = false;
        instance->on_message_received_callback = NULL;
        instance->on_message_received_context = NULL;

        // Codes_SRS_IOTHUBTRANSPORT_AMQP_MESSENGER_09_042: [If no failures occurr, amqp_messenger_unsubscribe_for_messages() shall return 0]
        result = RESULT_OK;
    }

    return result;
}

int amqp_messenger_send_message_disposition(AMQP_MESSENGER_HANDLE messenger_handle, AMQP_MESSENGER_MESSAGE_DISPOSITION_INFO* disposition_info, AMQP_MESSENGER_DISPOSITION_RESULT disposition_result)
{
    int result;

    // Codes_SRS_IOTHUBTRANSPORT_AMQP_MESSENGER_09_043: [If `messenger_handle` or `disposition_info` are NULL, amqp_messenger_send_message_disposition() shall fail and return non-zero value]
    if (messenger_handle == NULL || disposition_info == NULL)
    {
        LogError("Invalid argument (messenger_handle=%p, disposition_info=%p)", messenger_handle, disposition_info);
        result = MU_FAILURE;
    }
    // Codes_SRS_IOTHUBTRANSPORT_AMQP_MESSENGER_09_044: [If `disposition_info->source` is NULL, amqp_messenger_send_message_disposition() shall fail and return non-zero value]
    else if (disposition_info->source == NULL)
    {
        LogError("Failed sending message disposition (disposition_info->source is NULL)");
        result = MU_FAILURE;
    }
    else
    {
        AMQP_MESSENGER_INSTANCE* messenger = (AMQP_MESSENGER_INSTANCE*)messenger_handle;

        if (messenger->message_receiver == NULL)
        {
            // Codes_SRS_IOTHUBTRANSPORT_AMQP_MESSENGER_09_045: [If `messenger_handle->message_receiver` is NULL, amqp_messenger_send_message_disposition() shall fail and return non-zero value]
            LogError("Failed sending message disposition (message_receiver is not created; check if it is subscribed)");
            result = MU_FAILURE;
        }
        else
        {
            AMQP_VALUE uamqp_disposition_result;

            // Codes_SRS_IOTHUBTRANSPORT_AMQP_MESSENGER_09_046: [An AMQP_VALUE disposition result shall be created corresponding to the `disposition_result` provided]
            if ((uamqp_disposition_result = create_uamqp_disposition_result_from(disposition_result)) == NULL)
            {
                LogError("Failed sending message disposition (disposition result %d is not supported)", disposition_result);
                result = MU_FAILURE;
            }
            else
            {
                // Codes_SRS_IOTHUBTRANSPORT_AMQP_MESSENGER_09_047: [`messagereceiver_send_message_disposition()` shall be invoked passing `disposition_info->source`, `disposition_info->message_id` and the AMQP_VALUE disposition result]
                if (messagereceiver_send_message_disposition(messenger->message_receiver, disposition_info->source, disposition_info->message_id, uamqp_disposition_result) != RESULT_OK)
                {
                    // Codes_SRS_IOTHUBTRANSPORT_AMQP_MESSENGER_09_048: [If `messagereceiver_send_message_disposition()` fails, amqp_messenger_send_message_disposition() shall fail and return non-zero value]
                    LogError("Failed sending message disposition (messagereceiver_send_message_disposition failed)");
                    result = MU_FAILURE;
                }
                else
                {
                    destroy_message_disposition_info(disposition_info);

                    // Codes_SRS_IOTHUBTRANSPORT_AMQP_MESSENGER_09_050: [If no failures occurr, amqp_messenger_send_message_disposition() shall return 0]
                    result = RESULT_OK;
                }

                // Codes_SRS_IOTHUBTRANSPORT_AMQP_MESSENGER_09_049: [amqp_messenger_send_message_disposition() shall destroy the AMQP_VALUE disposition result]
                amqpvalue_destroy(uamqp_disposition_result);
            }
        }
    }

    return result;
}

int amqp_messenger_send_async(AMQP_MESSENGER_HANDLE messenger_handle, MESSAGE_HANDLE message, AMQP_MESSENGER_SEND_COMPLETE_CALLBACK on_user_defined_send_complete_callback, void* user_context)
{
    int result;

    if (messenger_handle == NULL || message == NULL || on_user_defined_send_complete_callback == NULL)
    {
        // Codes_SRS_IOTHUBTRANSPORT_AMQP_MESSENGER_09_010: [If `messenger_handle`, `message` or `on_event_send_complete_callback` are NULL, amqp_messenger_send_async() shall fail and return a non-zero value]
        LogError("Invalid argument (messenger_handle=%p, message=%p, on_user_defined_send_complete_callback=%p)", messenger_handle, message, on_user_defined_send_complete_callback);
        result = MU_FAILURE;
    }
    else
    {
        MESSAGE_HANDLE cloned_message;

        // Codes_SRS_IOTHUBTRANSPORT_AMQP_MESSENGER_09_011: [`message` shall be cloned using message_clone()]
        if ((cloned_message = message_clone(message)) == NULL)
        {
            // Codes_SRS_IOTHUBTRANSPORT_AMQP_MESSENGER_09_012: [If message_clone() fails, amqp_messenger_send_async() shall fail and return a non-zero value]
            LogError("Failed cloning AMQP message");
            result = MU_FAILURE;
        }
        else
        {
            MESSAGE_SEND_CONTEXT* message_context;
            AMQP_MESSENGER_INSTANCE *instance = (AMQP_MESSENGER_INSTANCE*)messenger_handle;

            // Codes_SRS_IOTHUBTRANSPORT_AMQP_MESSENGER_09_013: [amqp_messenger_send_async() shall allocate memory for a MESSAGE_SEND_CONTEXT structure (aka `send_ctx`)]
            if ((message_context = create_message_send_context()) == NULL)
            {
                // Codes_SRS_IOTHUBTRANSPORT_AMQP_MESSENGER_09_014: [If malloc() fails, amqp_messenger_send_async() shall fail and return a non-zero value]
                LogError("Failed creating context for sending message");
                message_destroy(cloned_message);
                result = MU_FAILURE;
            }
            else
            {
                // Codes_SRS_IOTHUBTRANSPORT_AMQP_MESSENGER_09_015: [The cloned `message`, callback and context shall be saved in ``send_ctx`]
                message_context->message = cloned_message;
                message_context->messenger = instance;
                message_context->on_send_complete_callback = on_user_defined_send_complete_callback;
                message_context->user_context = user_context;

                // Codes_SRS_IOTHUBTRANSPORT_AMQP_MESSENGER_09_016: [`send_ctx` shall be added to `instance->send_queue` using message_queue_add(), passing `on_message_processing_completed_callback`]
                if (message_queue_add(instance->send_queue, (MQ_MESSAGE_HANDLE)cloned_message, on_message_processing_completed_callback, (void*)message_context) != RESULT_OK)
                {
                    // Codes_SRS_IOTHUBTRANSPORT_AMQP_MESSENGER_09_017: [If message_queue_add() fails, amqp_messenger_send_async() shall fail and return a non-zero value]
                    LogError("Failed adding message to send queue");
                    // Codes_SRS_IOTHUBTRANSPORT_AMQP_MESSENGER_09_018: [If any failure occurs, amqp_messenger_send_async() shall free any memory it has allocated]
                    destroy_message_send_context(message_context);
                    message_destroy(cloned_message);
                    result = MU_FAILURE;
                }
                else
                {
                    // Codes_SRS_IOTHUBTRANSPORT_AMQP_MESSENGER_09_019: [If no failures occur, amqp_messenger_send_async() shall return zero]
                    result = RESULT_OK;
                }
            }
        }
    }

    return result;
}

int amqp_messenger_get_send_status(AMQP_MESSENGER_HANDLE messenger_handle, AMQP_MESSENGER_SEND_STATUS* send_status)
{
    int result;

    // Codes_SRS_IOTHUBTRANSPORT_AMQP_MESSENGER_09_029: [If `messenger_handle` or `send_status` are NULL, amqp_messenger_get_send_status() shall fail and return a non-zero value]
    if (messenger_handle == NULL || send_status == NULL)
    {
        LogError("Invalid argument (messenger_handle=%p, send_status=%p)", messenger_handle, send_status);
        result = MU_FAILURE;
    }
    else
    {
        AMQP_MESSENGER_INSTANCE* instance = (AMQP_MESSENGER_INSTANCE*)messenger_handle;
        bool is_empty;

        // Codes_SRS_IOTHUBTRANSPORT_AMQP_MESSENGER_09_030: [message_queue_is_empty() shall be invoked for `instance->send_queue`]
        if (message_queue_is_empty(instance->send_queue, &is_empty) != 0)
        {
            // Codes_SRS_IOTHUBTRANSPORT_AMQP_MESSENGER_09_031: [If message_queue_is_empty() fails, amqp_messenger_get_send_status() shall fail and return a non-zero value]
            LogError("Failed verifying if send queue is empty");
            result = MU_FAILURE;
        }
        else
        {
            // Codes_SRS_IOTHUBTRANSPORT_AMQP_MESSENGER_09_032: [If message_queue_is_empty() returns `true`, `send_status` shall be set to AMQP_MESSENGER_SEND_STATUS_IDLE]
            // Codes_SRS_IOTHUBTRANSPORT_AMQP_MESSENGER_09_033: [Otherwise, `send_status` shall be set to AMQP_MESSENGER_SEND_STATUS_BUSY]
            *send_status = (is_empty ? AMQP_MESSENGER_SEND_STATUS_IDLE : AMQP_MESSENGER_SEND_STATUS_BUSY);
            // Codes_SRS_IOTHUBTRANSPORT_AMQP_MESSENGER_09_034: [If no failures occur, amqp_messenger_get_send_status() shall return 0]
            result = RESULT_OK;
        }
    }

    return result;
}

int amqp_messenger_start(AMQP_MESSENGER_HANDLE messenger_handle, SESSION_HANDLE session_handle)
{
    int result;

    // Codes_SRS_IOTHUBTRANSPORT_AMQP_MESSENGER_09_051: [If `messenger_handle` or `session_handle` are NULL, amqp_messenger_start() shall fail and return non-zero value]
    if (messenger_handle == NULL || session_handle == NULL)
    {
        LogError("Invalid argument (session_handle is NULL)");
        result = MU_FAILURE;
    }
    else
    {
        AMQP_MESSENGER_INSTANCE* instance = (AMQP_MESSENGER_INSTANCE*)messenger_handle;

        // Codes_SRS_IOTHUBTRANSPORT_AMQP_MESSENGER_09_052: [If `instance->state` is not AMQP_MESSENGER_STATE_STOPPED, amqp_messenger_start() shall fail and return non-zero value]
        if (instance->state != AMQP_MESSENGER_STATE_STOPPED)
        {
            result = MU_FAILURE;
            LogError("amqp_messenger_start failed (current state is %d; expected AMQP_MESSENGER_STATE_STOPPED)", instance->state);
        }
        else
        {
            // Codes_SRS_IOTHUBTRANSPORT_AMQP_MESSENGER_09_053: [`session_handle` shall be saved on `instance->session_handle`]
            instance->session_handle = session_handle;

            // Codes_SRS_IOTHUBTRANSPORT_AMQP_MESSENGER_09_054: [If no failures occurr, `instance->state` shall be set to AMQP_MESSENGER_STATE_STARTING, and `instance->on_state_changed_callback` invoked if provided]
            update_messenger_state(instance, AMQP_MESSENGER_STATE_STARTING);

            // Codes_SRS_IOTHUBTRANSPORT_AMQP_MESSENGER_09_055: [If no failures occurr, amqp_messenger_start() shall return 0]
            result = RESULT_OK;
        }
    }

    return result;
}

int amqp_messenger_stop(AMQP_MESSENGER_HANDLE messenger_handle)
{
    int result;

    // Codes_SRS_IOTHUBTRANSPORT_AMQP_MESSENGER_09_056: [If `messenger_handle` is NULL, amqp_messenger_stop() shall fail and return non-zero value]
    if (messenger_handle == NULL)
    {
        result = MU_FAILURE;
        LogError("Invalid argument (messenger_handle is NULL)");
    }
    else
    {
        AMQP_MESSENGER_INSTANCE* instance = (AMQP_MESSENGER_INSTANCE*)messenger_handle;

        // Codes_SRS_IOTHUBTRANSPORT_AMQP_MESSENGER_09_057: [If `instance->state` is AMQP_MESSENGER_STATE_STOPPED, amqp_messenger_stop() shall fail and return non-zero value]
        if (instance->state == AMQP_MESSENGER_STATE_STOPPED)
        {
            result = MU_FAILURE;
            LogError("amqp_messenger_stop failed (messenger is already stopped)");
        }
        else
        {
            // Codes_SRS_IOTHUBTRANSPORT_AMQP_MESSENGER_09_058: [`instance->state` shall be set to AMQP_MESSENGER_STATE_STOPPING, and `instance->on_state_changed_callback` invoked if provided]
            update_messenger_state(instance, AMQP_MESSENGER_STATE_STOPPING);

            // Codes_SRS_IOTHUBTRANSPORT_AMQP_MESSENGER_09_059: [`instance->message_sender` and `instance->message_receiver` shall be destroyed along with all its links]
            destroy_message_sender(instance);
            destroy_message_receiver(instance);

            // Codes_SRS_IOTHUBTRANSPORT_AMQP_MESSENGER_09_060: [`instance->send_queue` items shall be rolled back using message_queue_move_all_back_to_pending()]
            if (message_queue_move_all_back_to_pending(instance->send_queue) != RESULT_OK)
            {
                LogError("Messenger failed to move events in progress back to wait_to_send list");

                // Codes_SRS_IOTHUBTRANSPORT_AMQP_MESSENGER_09_061: [If message_queue_move_all_back_to_pending() fails, amqp_messenger_stop() shall change the messenger state to AMQP_MESSENGER_STATE_ERROR and return a non-zero value]
                update_messenger_state(instance, AMQP_MESSENGER_STATE_ERROR);
                result = MU_FAILURE;
            }
            else
            {
                // Codes_SRS_IOTHUBTRANSPORT_AMQP_MESSENGER_09_062: [`instance->state` shall be set to AMQP_MESSENGER_STATE_STOPPED, and `instance->on_state_changed_callback` invoked if provided]
                update_messenger_state(instance, AMQP_MESSENGER_STATE_STOPPED);
                // Codes_SRS_IOTHUBTRANSPORT_AMQP_MESSENGER_09_063: [If no failures occurr, amqp_messenger_stop() shall return 0]
                result = RESULT_OK;
            }
        }
    }

    return result;
}

// @brief
//     Sets the messenger module state based on the state changes from messagesender and messagereceiver
static void process_state_changes(AMQP_MESSENGER_INSTANCE* instance)
{
    // Note: messagesender and messagereceiver are still not created or already destroyed
    //       when state is AMQP_MESSENGER_STATE_STOPPED, so no checking is needed there.

    if (instance->state == AMQP_MESSENGER_STATE_STARTED)
    {
        if (instance->message_sender_current_state != MESSAGE_SENDER_STATE_OPEN)
        {
            LogError("messagesender reported unexpected state %d while messenger was started", instance->message_sender_current_state);
            update_messenger_state(instance, AMQP_MESSENGER_STATE_ERROR);
        }
        else if (instance->message_receiver != NULL && instance->message_receiver_current_state != MESSAGE_RECEIVER_STATE_OPEN)
        {
            if (instance->message_receiver_current_state == MESSAGE_RECEIVER_STATE_OPENING)
            {
                bool is_timed_out;
                if (is_timeout_reached(instance->last_message_receiver_state_change_time, MAX_MESSAGE_RECEIVER_STATE_CHANGE_TIMEOUT_SECS, &is_timed_out) != RESULT_OK)
                {
                    LogError("messenger got an error (failed to verify messagereceiver start timeout)");
                    update_messenger_state(instance, AMQP_MESSENGER_STATE_ERROR);
                }
                else if (is_timed_out)
                {
                    LogError("messenger got an error (messagereceiver failed to start within expected timeout (%d secs))", MAX_MESSAGE_RECEIVER_STATE_CHANGE_TIMEOUT_SECS);
                    update_messenger_state(instance, AMQP_MESSENGER_STATE_ERROR);
                }
            }
            else if (instance->message_receiver_current_state == MESSAGE_RECEIVER_STATE_ERROR ||
                instance->message_receiver_current_state == MESSAGE_RECEIVER_STATE_IDLE)
            {
                LogError("messagereceiver reported unexpected state %d while messenger is starting", instance->message_receiver_current_state);
                update_messenger_state(instance, AMQP_MESSENGER_STATE_ERROR);
            }
        }
    }
    else
    {
        if (instance->state == AMQP_MESSENGER_STATE_STARTING)
        {
            if (instance->message_sender_current_state == MESSAGE_SENDER_STATE_OPEN)
            {
                update_messenger_state(instance, AMQP_MESSENGER_STATE_STARTED);
            }
            else if (instance->message_sender_current_state == MESSAGE_SENDER_STATE_OPENING)
            {
                bool is_timed_out;
                if (is_timeout_reached(instance->last_message_sender_state_change_time, MAX_MESSAGE_SENDER_STATE_CHANGE_TIMEOUT_SECS, &is_timed_out) != RESULT_OK)
                {
                    LogError("messenger failed to start (failed to verify messagesender start timeout)");
                    update_messenger_state(instance, AMQP_MESSENGER_STATE_ERROR);
                }
                else if (is_timed_out)
                {
                    LogError("messenger failed to start (messagesender failed to start within expected timeout (%d secs))", MAX_MESSAGE_SENDER_STATE_CHANGE_TIMEOUT_SECS);
                    update_messenger_state(instance, AMQP_MESSENGER_STATE_ERROR);
                }
            }
            // For this module, the only valid scenario where messagesender state is IDLE is if
            // the messagesender hasn't been created yet or already destroyed.
            else if ((instance->message_sender_current_state == MESSAGE_SENDER_STATE_ERROR) ||
                (instance->message_sender_current_state == MESSAGE_SENDER_STATE_CLOSING) ||
                (instance->message_sender_current_state == MESSAGE_SENDER_STATE_IDLE && instance->message_sender != NULL))
            {
                LogError("messagesender reported unexpected state %d while messenger is starting", instance->message_sender_current_state);
                update_messenger_state(instance, AMQP_MESSENGER_STATE_ERROR);
            }
        }
        // message sender and receiver are stopped/destroyed synchronously, so no need for state control.
    }
}

static void manage_amqp_messengers(AMQP_MESSENGER_INSTANCE* msgr)
{
    if (msgr->state == AMQP_MESSENGER_STATE_STARTING)
    {
        if (msgr->message_sender == NULL)
        {
            if (create_message_sender(msgr) != RESULT_OK)
            {
                update_messenger_state(msgr, AMQP_MESSENGER_STATE_ERROR);
            }
        }
    }
    else if (msgr->state == AMQP_MESSENGER_STATE_STARTED)
    {
        if (msgr->receive_messages == true &&
            msgr->message_receiver == NULL &&
            create_message_receiver(msgr) != RESULT_OK)
        {
            LogError("amqp_messenger_do_work warning (failed creating the message receiver [%s])", msgr->config->device_id);
        }
        else if (msgr->receive_messages == false && msgr->message_receiver != NULL)
        {
            destroy_message_receiver(msgr);
        }
    }
}

static void handle_errors_and_timeouts(AMQP_MESSENGER_INSTANCE* msgr)
{
    if (msgr->send_error_count >= msgr->max_send_error_count)
    {
        LogError("Reached max number of consecutive send failures (%s, %lu)", msgr->config->device_id, (unsigned long)msgr->max_send_error_count);
        update_messenger_state(msgr, AMQP_MESSENGER_STATE_ERROR);
    }
}

void amqp_messenger_do_work(AMQP_MESSENGER_HANDLE messenger_handle)
{
    if (messenger_handle == NULL)
    {
        // Codes_SRS_IOTHUBTRANSPORT_AMQP_MESSENGER_09_064: [If `messenger_handle` is NULL, amqp_messenger_do_work() shall fail and return]
        LogError("Invalid argument (messenger_handle is NULL)");
    }
    else
    {
        AMQP_MESSENGER_INSTANCE* msgr = (AMQP_MESSENGER_INSTANCE*)messenger_handle;

        // Codes_SRS_IOTHUBTRANSPORT_AMQP_MESSENGER_09_065: [amqp_messenger_do_work() shall update the current state according to the states of message sender and receiver]
        // Codes_SRS_IOTHUBTRANSPORT_AMQP_MESSENGER_09_068: [If the message sender state changes to MESSAGE_SENDER_STATE_OPEN, amqp_messenger_do_work() shall set the state to AMQP_MESSENGER_STATE_STARTED]
        process_state_changes(msgr);

        // Codes_SRS_IOTHUBTRANSPORT_AMQP_MESSENGER_09_066: [If the current state is AMQP_MESSENGER_STARTING, amqp_messenger_do_work() shall create and start `instance->message_sender`]
        // Codes_SRS_IOTHUBTRANSPORT_AMQP_MESSENGER_09_067: [If the `instance->message_sender` fails to be created/started, amqp_messenger_do_work() shall set the state to AMQP_MESSENGER_STATE_ERROR]
        // Codes_SRS_IOTHUBTRANSPORT_AMQP_MESSENGER_09_069: [If the `instance->receive_messages` is true, amqp_messenger_do_work() shall create and start `instance->message_receiver` if not done before]
        // Codes_SRS_IOTHUBTRANSPORT_AMQP_MESSENGER_09_070: [If the `instance->message_receiver` fails to be created/started, amqp_messenger_do_work() shall set the state to AMQP_MESSENGER_STATE_ERROR]
        // Codes_SRS_IOTHUBTRANSPORT_AMQP_MESSENGER_09_071: [If the `instance->receive_messages` is false, amqp_messenger_do_work() shall stop and destroy `instance->message_receiver` if not done before]
        // Codes_SRS_IOTHUBTRANSPORT_AMQP_MESSENGER_09_072: [If the `instance->message_receiver` fails to be stopped/destroyed, amqp_messenger_do_work() shall set the state to AMQP_MESSENGER_STATE_ERROR]
        manage_amqp_messengers(msgr);

        // Codes_SRS_IOTHUBTRANSPORT_AMQP_MESSENGER_09_073: [amqp_messenger_do_work() shall invoke message_queue_do_work() on `instance->send_queue`]
        if (msgr->state == AMQP_MESSENGER_STATE_STARTED)
        {
            message_queue_do_work(msgr->send_queue);
        }

        // Codes_SRS_IOTHUBTRANSPORT_AMQP_MESSENGER_09_074: [If `instance->send_error_count` DEFAULT_MAX_SEND_ERROR_COUNT (10), amqp_messenger_do_work() shall change the state to AMQP_MESSENGER_STATE_ERROR]
        handle_errors_and_timeouts(msgr);
    }
}

void amqp_messenger_destroy(AMQP_MESSENGER_HANDLE messenger_handle)
{
    // Codes_SRS_IOTHUBTRANSPORT_AMQP_MESSENGER_09_123: [If `messenger_handle` is NULL, amqp_messenger_destroy() shall fail and return]
    if (messenger_handle == NULL)
    {
        LogError("invalid argument (messenger_handle is NULL)");
    }
    else
    {
        AMQP_MESSENGER_INSTANCE* instance = (AMQP_MESSENGER_INSTANCE*)messenger_handle;

        // Codes_SRS_IOTHUBTRANSPORT_AMQP_MESSENGER_09_124: [If `instance->state` is not AMQP_MESSENGER_STATE_STOPPED, amqp_messenger_destroy() shall invoke amqp_messenger_stop()]
        if (instance->state != AMQP_MESSENGER_STATE_STOPPED)
        {
            (void)amqp_messenger_stop(messenger_handle);
        }

        if (instance->send_queue != NULL)
        {
            // Codes_SRS_IOTHUBTRANSPORT_AMQP_MESSENGER_09_125: [message_queue_destroy() shall be invoked for `instance->send_queue`]
            message_queue_destroy(instance->send_queue);
        }

        // Codes_SRS_IOTHUBTRANSPORT_AMQP_MESSENGER_09_126: [amqp_messenger_destroy() shall release all the memory allocated for `instance`]
        destroy_configuration(instance->config);

        free((void*)instance);
    }
}

AMQP_MESSENGER_HANDLE amqp_messenger_create(const AMQP_MESSENGER_CONFIG* messenger_config)
{
    AMQP_MESSENGER_HANDLE handle;

    // Codes_SRS_IOTHUBTRANSPORT_AMQP_MESSENGER_09_001: [If `messenger_config` is NULL, amqp_messenger_create() shall return NULL]
    if (!is_valid_configuration(messenger_config))
    {
        // Codes_SRS_IOTHUBTRANSPORT_AMQP_MESSENGER_09_002: [If `messenger_config`'s `device_id`, `iothub_host_fqdn`, `receive_link.source_suffix` or `send_link.target_suffix` are NULL, amqp_messenger_create() shall return NULL]
        handle = NULL;
    }
    else
    {
        AMQP_MESSENGER_INSTANCE* instance;

        // Codes_SRS_IOTHUBTRANSPORT_AMQP_MESSENGER_09_003: [amqp_messenger_create() shall allocate memory for the messenger instance structure (aka `instance`)]
        if ((instance = (AMQP_MESSENGER_INSTANCE*)malloc(sizeof(AMQP_MESSENGER_INSTANCE))) == NULL)
        {
            // Codes_SRS_IOTHUBTRANSPORT_AMQP_MESSENGER_09_004: [If malloc() fails, amqp_messenger_create() shall fail and return NULL]
            LogError("Failed allocating AMQP_MESSENGER_INSTANCE");
            handle = NULL;
        }
        else
        {
            memset(instance, 0, sizeof(AMQP_MESSENGER_INSTANCE));

            // Codes_SRS_IOTHUBTRANSPORT_AMQP_MESSENGER_09_005: [amqp_messenger_create() shall save a copy of `messenger_config` into `instance`]
            if ((instance->config = clone_configuration(messenger_config)) == NULL)
            {
                // Codes_SRS_IOTHUBTRANSPORT_AMQP_MESSENGER_09_006: [If the copy fails, amqp_messenger_create() shall fail and return NULL]
                LogError("Failed copying AMQP messenger configuration");
                handle = NULL;
            }
            else
            {
                MESSAGE_QUEUE_CONFIG mq_config;
                mq_config.max_retry_count = DEFAULT_EVENT_SEND_RETRY_LIMIT;
                mq_config.max_message_enqueued_time_secs = DEFAULT_EVENT_SEND_TIMEOUT_SECS;
                mq_config.max_message_processing_time_secs = 0;
                mq_config.on_process_message_callback = on_process_message_callback;

                // Codes_SRS_IOTHUBTRANSPORT_AMQP_MESSENGER_09_007: [`instance->send_queue` shall be set using message_queue_create(), passing `on_process_message_callback`]
                if ((instance->send_queue = message_queue_create(&mq_config)) == NULL)
                {
                    // Codes_SRS_IOTHUBTRANSPORT_AMQP_MESSENGER_09_008: [If message_queue_create() fails, amqp_messenger_create() shall fail and return NULL]
                    LogError("Failed creating message queue");
                    handle = NULL;
                }
                else
                {
                    instance->state = AMQP_MESSENGER_STATE_STOPPED;
                    instance->message_sender_current_state = MESSAGE_SENDER_STATE_IDLE;
                    instance->message_sender_previous_state = MESSAGE_SENDER_STATE_IDLE;
                    instance->message_receiver_current_state = MESSAGE_RECEIVER_STATE_IDLE;
                    instance->message_receiver_previous_state = MESSAGE_RECEIVER_STATE_IDLE;
                    instance->last_message_sender_state_change_time = INDEFINITE_TIME;
                    instance->last_message_receiver_state_change_time = INDEFINITE_TIME;
                    instance->max_send_error_count = DEFAULT_MAX_SEND_ERROR_COUNT;
                    instance->receive_messages = false;

                    // Codes_SRS_IOTHUBTRANSPORT_AMQP_MESSENGER_09_009: [If no failures occurr, amqp_messenger_create() shall return a handle to `instance`]
                    handle = (AMQP_MESSENGER_HANDLE)instance;
                }
            }
        }

        if (handle == NULL)
        {
            amqp_messenger_destroy((AMQP_MESSENGER_HANDLE)instance);
        }
    }

    return handle;
}

int amqp_messenger_set_option(AMQP_MESSENGER_HANDLE messenger_handle, const char* name, void* value)
{
    int result;

    // Codes_SRS_IOTHUBTRANSPORT_AMQP_MESSENGER_09_127: [If `messenger_handle` or `name` or `value` is NULL, amqp_messenger_set_option shall fail and return a non-zero value]
    if (messenger_handle == NULL || name == NULL || value == NULL)
    {
        LogError("Invalid argument (messenger_handle=%p, name=%p, value=%p)",
            messenger_handle, name, value);
        result = MU_FAILURE;
    }
    else
    {
        AMQP_MESSENGER_INSTANCE* instance = (AMQP_MESSENGER_INSTANCE*)messenger_handle;

        // Codes_SRS_IOTHUBTRANSPORT_AMQP_MESSENGER_09_128: [If name matches AMQP_MESSENGER_OPTION_EVENT_SEND_TIMEOUT_SECS, `value` shall be set on `instance->send_queue` using message_queue_set_max_message_enqueued_time_secs()]
        if (strcmp(AMQP_MESSENGER_OPTION_EVENT_SEND_TIMEOUT_SECS, name) == 0)
        {
            if (message_queue_set_max_message_enqueued_time_secs(instance->send_queue, *(size_t*)value) != 0)
            {
                // Codes_SRS_IOTHUBTRANSPORT_AMQP_MESSENGER_09_129: [If message_queue_set_max_message_enqueued_time_secs() fails, amqp_messenger_set_option() shall fail and return a non-zero value]
                LogError("Failed setting option %s", AMQP_MESSENGER_OPTION_EVENT_SEND_TIMEOUT_SECS);
                result = MU_FAILURE;
            }
            else
            {
                // Codes_SRS_IOTHUBTRANSPORT_AMQP_MESSENGER_09_131: [If no errors occur, amqp_messenger_set_option shall return 0]
                result = RESULT_OK;
            }
        }
        else if(strcmp(OPTION_PRODUCT_INFO, name) == 0)
        {
            if (Map_AddOrUpdate(instance->config->send_link.attach_properties, CLIENT_VERSION_PROPERTY_NAME, value) == MAP_OK)
            {
                if (Map_AddOrUpdate(instance->config->receive_link.attach_properties, CLIENT_VERSION_PROPERTY_NAME, value) == MAP_OK)
                {
                    result = RESULT_OK;
                }
                else
                {
                    LogError("Failed setting option %s for receive link", CLIENT_VERSION_PROPERTY_NAME);
                    result = MU_FAILURE;
                }
            }
            else
            {
                LogError("Failed setting option %s for send link", CLIENT_VERSION_PROPERTY_NAME);
                result = MU_FAILURE;
            }
        }
        else
        {
            // Codes_SRS_IOTHUBTRANSPORT_AMQP_MESSENGER_09_130: [If `name` does not match any supported option, amqp_messenger_set_option() shall fail and return a non-zero value]
            LogError("Invalid argument (option '%s' is not valid)", name);
            result = MU_FAILURE;
        }
    }

    return result;
}

OPTIONHANDLER_HANDLE amqp_messenger_retrieve_options(AMQP_MESSENGER_HANDLE messenger_handle)
{
    OPTIONHANDLER_HANDLE result;

    // Codes_SRS_IOTHUBTRANSPORT_AMQP_MESSENGER_09_132: [If `messenger_handle` is NULL, amqp_messenger_retrieve_options shall fail and return NULL]
    if (messenger_handle == NULL)
    {
        LogError("Invalid argument (messenger_handle is NULL)");
        result = NULL;
    }
    else
    {
        // Codes_SRS_IOTHUBTRANSPORT_AMQP_MESSENGER_09_133: [An OPTIONHANDLER_HANDLE instance shall be created using OptionHandler_Create]
        result = OptionHandler_Create(amqp_messenger_clone_option, amqp_messenger_destroy_option, (pfSetOption)amqp_messenger_set_option);

        if (result == NULL)
        {
            // Codes_SRS_IOTHUBTRANSPORT_AMQP_MESSENGER_09_134: [If an OPTIONHANDLER_HANDLE instance fails to be created, amqp_messenger_retrieve_options shall fail and return NULL]
            LogError("Failed to retrieve options from messenger instance (OptionHandler_Create failed)");
        }
        else
        {
            AMQP_MESSENGER_INSTANCE* instance = (AMQP_MESSENGER_INSTANCE*)messenger_handle;
            OPTIONHANDLER_HANDLE mq_options;

            // Codes_SRS_IOTHUBTRANSPORT_AMQP_MESSENGER_09_135: [`instance->send_queue` options shall be retrieved using message_queue_retrieve_options()]
            if ((mq_options = message_queue_retrieve_options(instance->send_queue)) == NULL)
            {
                // Codes_SRS_IOTHUBTRANSPORT_AMQP_MESSENGER_09_136: [If message_queue_retrieve_options() fails, amqp_messenger_retrieve_options shall fail and return NULL]
                LogError("failed to retrieve options from send queue)");
                // Codes_SRS_IOTHUBTRANSPORT_AMQP_MESSENGER_09_139: [If amqp_messenger_retrieve_options fails, any allocated memory shall be freed]
                OptionHandler_Destroy(result);
                result = NULL;
            }
            // Codes_SRS_IOTHUBTRANSPORT_AMQP_MESSENGER_09_137: [Each option of `instance` shall be added to the OPTIONHANDLER_HANDLE instance using OptionHandler_AddOption]
            else if (OptionHandler_AddOption(result, MESSENGER_SAVED_MQ_OPTIONS, (void*)mq_options) != OPTIONHANDLER_OK)
            {
                // Codes_SRS_IOTHUBTRANSPORT_AMQP_MESSENGER_09_138: [If OptionHandler_AddOption fails, amqp_messenger_retrieve_options shall fail and return NULL]
                LogError("failed adding option '%s'", MESSENGER_SAVED_MQ_OPTIONS);
                // Codes_SRS_IOTHUBTRANSPORT_AMQP_MESSENGER_09_139: [If amqp_messenger_retrieve_options fails, any allocated memory shall be freed]
                OptionHandler_Destroy(mq_options);
                OptionHandler_Destroy(result);
                result = NULL;
            }
        }
    }

    // Codes_SRS_IOTHUBTRANSPORT_AMQP_MESSENGER_09_140: [If no failures occur, amqp_messenger_retrieve_options shall return the OPTIONHANDLER_HANDLE instance]
    return result;
}

void amqp_messenger_destroy_disposition_info(AMQP_MESSENGER_MESSAGE_DISPOSITION_INFO* disposition_info)
{
    // Codes_SRS_IOTHUBTRANSPORT_AMQP_MESSENGER_09_132: [If `disposition_info` is NULL, amqp_messenger_destroy_disposition_info shall return immediately]
    if (disposition_info == NULL)
    {
        LogError("Invalid argument (disposition_info is NULL)");
    }
    else
    {
        // Codes_SRS_IOTHUBTRANSPORT_AMQP_MESSENGER_09_132: [All memory allocated for `disposition_info` shall be released]
        destroy_message_disposition_info(disposition_info);
    }
}
