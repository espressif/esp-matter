// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>
#include <stdbool.h>
#include "azure_c_shared_utility/optimize_size.h"
#include "azure_c_shared_utility/crt_abstractions.h"
#include "azure_c_shared_utility/gballoc.h"
#include "azure_c_shared_utility/agenttime.h"
#include "azure_c_shared_utility/xlogging.h"
#include "azure_c_shared_utility/singlylinkedlist.h"

typedef struct MESSAGE_QUEUE_TAG MESSAGE_QUEUE;

#include "internal/message_queue.h"

#define RESULT_OK 0
#define INDEFINITE_TIME ((time_t)(-1))

static const char* SAVED_OPTION_MAX_RETRY_COUNT = "SAVED_OPTION_MAX_RETRY_COUNT";
static const char* SAVED_OPTION_MAX_ENQUEUE_TIME_SECS = "SAVED_OPTION_MAX_ENQUEUE_TIME_SECS";
static const char* SAVED_OPTION_MAX_PROCESSING_TIME_SECS = "SAVED_OPTION_MAX_PROCESSING_TIME_SECS";


struct MESSAGE_QUEUE_TAG
{
    size_t max_message_enqueued_time_secs;
    size_t max_message_processing_time_secs;
    size_t max_retry_count;

    PROCESS_MESSAGE_CALLBACK on_process_message_callback;
    void* on_process_message_context;

    SINGLYLINKEDLIST_HANDLE pending;
    SINGLYLINKEDLIST_HANDLE in_progress;
};

typedef struct MESSAGE_QUEUE_ITEM_TAG
{
    MQ_MESSAGE_HANDLE message;
    MESSAGE_PROCESSING_COMPLETED_CALLBACK on_message_processing_completed_callback;
    void* user_context;
    time_t enqueue_time;
    time_t processing_start_time;
    size_t number_of_attempts;
} MESSAGE_QUEUE_ITEM;



// ---------- Helper Functions ---------- //

static bool find_item_by_message_ptr(LIST_ITEM_HANDLE list_item, const void* match_context)
{
    bool result;
    MESSAGE_QUEUE_ITEM* current_item = (MESSAGE_QUEUE_ITEM*)singlylinkedlist_item_get_value(list_item);
    MQ_MESSAGE_HANDLE* target_item = (MQ_MESSAGE_HANDLE*)match_context;

    if (current_item == NULL)
    {
        LogError("Failed finding item: current_item is NULL");
        result = false;
    }
    else
    {
        result = (current_item->message == target_item);
    }
    return result;
}

static void fire_message_callback(MESSAGE_QUEUE_ITEM* mq_item, MESSAGE_QUEUE_RESULT result, void* reason)
{
    if (mq_item->on_message_processing_completed_callback != NULL)
    {
        if (result == MESSAGE_QUEUE_RETRYABLE_ERROR)
        {
            result = MESSAGE_QUEUE_ERROR;
        }

        mq_item->on_message_processing_completed_callback(mq_item->message, result, reason, mq_item->user_context);
    }
}

static bool should_retry_sending(MESSAGE_QUEUE_HANDLE message_queue, MESSAGE_QUEUE_ITEM* mq_item, MESSAGE_QUEUE_RESULT result)
{
    return (result == MESSAGE_QUEUE_RETRYABLE_ERROR && mq_item->number_of_attempts <= message_queue->max_retry_count);
}

static int retry_sending_message(MESSAGE_QUEUE_HANDLE message_queue, LIST_ITEM_HANDLE list_item)
{
    int result;
    MESSAGE_QUEUE_ITEM* mq_item;

    mq_item = (MESSAGE_QUEUE_ITEM*)singlylinkedlist_item_get_value(list_item);
    if (message_queue == NULL)
    {
        LogError("message_queue is NULL");
        result = MU_FAILURE;
    }
    else if (singlylinkedlist_remove(message_queue->in_progress, list_item))
    {
        LogError("Failed removing message from in-progress list");
        result = MU_FAILURE;
    }
    else if (singlylinkedlist_add(message_queue->pending, (const void*)mq_item) == NULL)
    {
        LogError("Failed moving message back to pending list");
        result = MU_FAILURE;
    }
    else
    {
        result = RESULT_OK;
    }

    return result;
}

static void dequeue_message_and_fire_callback(SINGLYLINKEDLIST_HANDLE list, LIST_ITEM_HANDLE list_item, MESSAGE_QUEUE_RESULT result, void* reason)
{
    MESSAGE_QUEUE_ITEM* mq_item = (MESSAGE_QUEUE_ITEM*)singlylinkedlist_item_get_value(list_item);

    // Codes_SRS_MESSAGE_QUEUE_09_045: [If `message` is present in `message_queue->in_progress`, it shall be removed]
    if (singlylinkedlist_remove(list, list_item))
    {
        LogError("failed removing message from list (%p)", list);
    }

    // Codes_SRS_MESSAGE_QUEUE_09_049: [Otherwise `mq_item->on_message_processing_completed_callback` shall be invoked passing `mq_item->message`, `result`, `reason` and `mq_item->user_context`]
    fire_message_callback(mq_item, result, reason);

    // Codes_SRS_MESSAGE_QUEUE_09_050: [The `mq_item` related to `message` shall be freed]
    free(mq_item);
}

static void on_process_message_completed_callback(MESSAGE_QUEUE_HANDLE message_queue, MQ_MESSAGE_HANDLE message, MESSAGE_QUEUE_RESULT result, USER_DEFINED_REASON reason)
{
    // Codes_SRS_MESSAGE_QUEUE_09_069: [If `message` or `message_queue` are NULL, on_message_processing_completed_callback shall return immediately]
    if (message == NULL || message_queue == NULL)
    {
        LogError("on_process_message_completed_callback invoked with NULL arguments (message=%p, message_queue=%p)", message, message_queue);
    }
    else
    {
        LIST_ITEM_HANDLE list_item;

        if ((list_item = singlylinkedlist_find(message_queue->in_progress, find_item_by_message_ptr, message)) == NULL)
        {
            // Codes_SRS_MESSAGE_QUEUE_09_044: [If `message` is not present in `message_queue->in_progress`, it shall be ignored]
            LogError("on_process_message_completed_callback invoked for a message not in the in-progress list (%p)", message);
        }
        else
        {
            MESSAGE_QUEUE_ITEM* mq_item = (MESSAGE_QUEUE_ITEM*)singlylinkedlist_item_get_value(list_item);

            // Codes_SRS_MESSAGE_QUEUE_09_047: [If `result` is MESSAGE_QUEUE_RETRYABLE_ERROR and `mq_item->number_of_attempts` is less than or equal `message_queue->max_retry_count`, the `message` shall be moved to `message_queue->pending` to be re-sent]
            // Codes_SRS_MESSAGE_QUEUE_09_048: [If `result` is MESSAGE_QUEUE_RETRYABLE_ERROR and `mq_item->number_of_attempts` is greater than `message_queue->max_retry_count`, result shall be changed to MESSAGE_QUEUE_ERROR]
            if (!should_retry_sending(message_queue, mq_item, result) || retry_sending_message(message_queue, list_item) != RESULT_OK)
            {
                dequeue_message_and_fire_callback(message_queue->in_progress, list_item, result, reason);
            }
        }
    }
}

static void process_timeouts(MESSAGE_QUEUE_HANDLE message_queue)
{
    time_t current_time;

    if ((current_time = get_time(NULL)) == INDEFINITE_TIME)
    {
        LogError("failed processing timeouts (get_time failed)");
    }
    else
    {
        // Codes_SRS_MESSAGE_QUEUE_09_035: [If `message_queue->max_message_enqueued_time_secs` is greater than zero, `message_queue->in_progress` and `message_queue->pending` items shall be checked for timeout]
        if (message_queue->max_message_enqueued_time_secs > 0)
        {
            LIST_ITEM_HANDLE list_item = singlylinkedlist_get_head_item(message_queue->pending);

            while (list_item != NULL)
            {
                LIST_ITEM_HANDLE current_list_item = list_item;
                MESSAGE_QUEUE_ITEM* mq_item = (MESSAGE_QUEUE_ITEM*)singlylinkedlist_item_get_value(current_list_item);

                list_item = singlylinkedlist_get_next_item(list_item);

                if (mq_item == NULL)
                {
                    LogError("failed processing timeouts (unexpected NULL pointer to MESSAGE_QUEUE_ITEM)");
                }
                else if (get_difftime(current_time, mq_item->enqueue_time) >= message_queue->max_message_enqueued_time_secs)
                {
                    // Codes_SRS_MESSAGE_QUEUE_09_036: [If any items are in `message_queue` lists for `message_queue->max_message_enqueued_time_secs` or more, they shall be removed and `message_queue->on_message_processing_completed_callback` invoked with MESSAGE_QUEUE_TIMEOUT]
                    dequeue_message_and_fire_callback(message_queue->pending, current_list_item, MESSAGE_QUEUE_TIMEOUT, NULL);
                }
                else
                {
                    // The pending list order is already based on enqueue time, so if one message is not expired, later ones won't be either.
                    break;
                }
            }

            list_item = singlylinkedlist_get_head_item(message_queue->in_progress);

            while (list_item != NULL)
            {
                LIST_ITEM_HANDLE current_list_item = list_item;
                MESSAGE_QUEUE_ITEM* mq_item = (MESSAGE_QUEUE_ITEM*)singlylinkedlist_item_get_value(current_list_item);

                list_item = singlylinkedlist_get_next_item(list_item);

                if (mq_item == NULL)
                {
                    LogError("failed processing timeouts (unexpected NULL pointer to MESSAGE_QUEUE_ITEM)");
                }
                else if (get_difftime(current_time, mq_item->enqueue_time) >= message_queue->max_message_enqueued_time_secs)
                {
                    // Codes_SRS_MESSAGE_QUEUE_09_038: [If any items are in `message_queue->in_progress` for `message_queue->max_message_processing_time_secs` or more, they shall be removed and `message_queue->on_message_processing_completed_callback` invoked with MESSAGE_QUEUE_TIMEOUT]
                    dequeue_message_and_fire_callback(message_queue->in_progress, current_list_item, MESSAGE_QUEUE_TIMEOUT, NULL);
                }
            }
        }

        // Codes_SRS_MESSAGE_QUEUE_09_037: [If `message_queue->max_message_processing_time_secs` is greater than zero, `message_queue->in_progress` items shall be checked for timeout]
        if (message_queue->max_message_processing_time_secs > 0)
        {
            LIST_ITEM_HANDLE list_item = singlylinkedlist_get_head_item(message_queue->in_progress);

            while (list_item != NULL)
            {
                LIST_ITEM_HANDLE current_list_item = list_item;
                MESSAGE_QUEUE_ITEM* mq_item = (MESSAGE_QUEUE_ITEM*)singlylinkedlist_item_get_value(current_list_item);

                list_item = singlylinkedlist_get_next_item(list_item);

                if (mq_item == NULL)
                {
                    LogError("failed processing timeouts (unexpected NULL pointer to MESSAGE_QUEUE_ITEM)");
                }
                else if (get_difftime(current_time, mq_item->processing_start_time) >= message_queue->max_message_processing_time_secs)
                {
                    dequeue_message_and_fire_callback(message_queue->in_progress, current_list_item, MESSAGE_QUEUE_TIMEOUT, NULL);
                }
                else
                {
                    // The in-progress list order is already based on start-processing time, so if one message is not expired, later ones won't be either.
                    break;
                }
            }
        }
    }
}

static void process_pending_messages(MESSAGE_QUEUE_HANDLE message_queue)
{
    LIST_ITEM_HANDLE list_item;

    while ((list_item = singlylinkedlist_get_head_item(message_queue->pending)) != NULL)
    {
        MESSAGE_QUEUE_ITEM* mq_item = (MESSAGE_QUEUE_ITEM*)singlylinkedlist_item_get_value(list_item);

        if (mq_item == NULL)
        {
            LogError("internal error, failed to retrieve list node value");
            break;
        }
        else if (singlylinkedlist_remove(message_queue->pending, list_item) != 0)
        {
            LogError("failed moving message out of pending list (%p)", mq_item->message);

            // Codes_SRS_MESSAGE_QUEUE_09_042: [If any failures occur, `mq_item->on_message_processing_completed_callback` shall be invoked with MESSAGE_QUEUE_ERROR and `mq_item` freed]
            if (mq_item->on_message_processing_completed_callback != NULL)
            {
                mq_item->on_message_processing_completed_callback(mq_item->message, MESSAGE_QUEUE_ERROR, NULL, mq_item->user_context);
            }

            // Not freeing since this would cause a memory A/V on the next call.

            break; // Trying to avoid an infinite loop
        }
        // Codes_SRS_MESSAGE_QUEUE_09_040: [`mq_item->processing_start_time` shall be set using get_time()]
        else if ((mq_item->processing_start_time = get_time(NULL)) == INDEFINITE_TIME)
        {
            // Codes_SRS_MESSAGE_QUEUE_09_041: [If get_time() fails, `mq_item` shall be removed from `message_queue->in_progress`]
            LogError("failed setting message processing_start_time (%p)", mq_item->message);

            // Codes_SRS_MESSAGE_QUEUE_09_042: [If any failures occur, `mq_item->on_message_processing_completed_callback` shall be invoked with MESSAGE_QUEUE_ERROR and `mq_item` freed]
            if (mq_item->on_message_processing_completed_callback != NULL)
            {
                mq_item->on_message_processing_completed_callback(mq_item->message, MESSAGE_QUEUE_ERROR, NULL, mq_item->user_context);
            }

            free(mq_item);
        }
        // Codes_SRS_MESSAGE_QUEUE_09_039: [Each `mq_item` in `message_queue->pending` shall be moved to `message_queue->in_progress`]
        else if (singlylinkedlist_add(message_queue->in_progress, (const void*)mq_item) == NULL)
        {
            LogError("failed moving message to in-progress list (%p)", mq_item->message);

            // Codes_SRS_MESSAGE_QUEUE_09_042: [If any failures occur, `mq_item->on_message_processing_completed_callback` shall be invoked with MESSAGE_QUEUE_ERROR and `mq_item` freed]
            if (mq_item->on_message_processing_completed_callback != NULL)
            {
                mq_item->on_message_processing_completed_callback(mq_item->message, MESSAGE_QUEUE_ERROR, NULL, mq_item->user_context);
            }

            free(mq_item);
        }
        else
        {
            mq_item->number_of_attempts++;

            // Codes_SRS_MESSAGE_QUEUE_09_043: [If no failures occur, `message_queue->on_process_message_callback` shall be invoked passing `mq_item->message` and `on_process_message_completed_callback`]
            message_queue->on_process_message_callback(message_queue, mq_item->message, on_process_message_completed_callback, mq_item->user_context);
        }
    }
}

static void* cloneOption(const char* name, const void* value)
{
    void* result;

    if (name == NULL || value == NULL)
    {
        LogError("invalid argument (name=%p, value=%p)", name, value);
        result = NULL;
    }
    else if (strcmp(SAVED_OPTION_MAX_ENQUEUE_TIME_SECS, name) == 0 ||
        strcmp(SAVED_OPTION_MAX_PROCESSING_TIME_SECS, name) == 0 ||
        strcmp(SAVED_OPTION_MAX_RETRY_COUNT, name) == 0)
    {
        if ((result = malloc(sizeof(size_t))) == NULL)
        {
            LogError("failed cloning option %s (malloc failed)", name);
        }
        else
        {
            memcpy(result, value, sizeof(size_t));
        }
    }
    else
    {
        LogError("option %s is invalid", name);
        result = NULL;
    }

    return result;
}

static void destroyOption(const char* name, const void* value)
{
    if (name == NULL || value == NULL)
    {
        LogError("invalid argument (name=%p, value=%p)", name, value);
    }
    else if (strcmp(SAVED_OPTION_MAX_ENQUEUE_TIME_SECS, name) == 0 ||
        strcmp(SAVED_OPTION_MAX_PROCESSING_TIME_SECS, name) == 0 ||
        strcmp(SAVED_OPTION_MAX_RETRY_COUNT, name) == 0)
    {
        free((void*)value);
    }
    else
    {
        LogError("option %s is invalid", name);
    }
}



// ---------- Public APIs ---------- //

void message_queue_remove_all(MESSAGE_QUEUE_HANDLE message_queue)
{
    // Codes_SRS_MESSAGE_QUEUE_09_026: [If `message_queue` is NULL, message_queue_retrieve_options shall return]
    if (message_queue != NULL)
    {
        LIST_ITEM_HANDLE list_item;

        // Codes_SRS_MESSAGE_QUEUE_09_027: [Each `mq_item` in `message_queue->pending` and `message_queue->in_progress` lists shall be removed]
        while ((list_item = singlylinkedlist_get_head_item(message_queue->in_progress)) != NULL)
        {
            // Codes_SRS_MESSAGE_QUEUE_09_028: [`message_queue->on_message_processing_completed_callback` shall be invoked with MESSAGE_QUEUE_CANCELLED for each `mq_item` removed]
            // Codes_SRS_MESSAGE_QUEUE_09_029: [Each `mq_item` shall be freed]
            dequeue_message_and_fire_callback(message_queue->in_progress, list_item, MESSAGE_QUEUE_CANCELLED, NULL);
        }

        while ((list_item = singlylinkedlist_get_head_item(message_queue->pending)) != NULL)
        {
            // Codes_SRS_MESSAGE_QUEUE_09_028: [`message_queue->on_message_processing_completed_callback` shall be invoked with MESSAGE_QUEUE_CANCELLED for each `mq_item` removed]
            // Codes_SRS_MESSAGE_QUEUE_09_029: [Each `mq_item` shall be freed]
            dequeue_message_and_fire_callback(message_queue->pending, list_item, MESSAGE_QUEUE_CANCELLED, NULL);
        }
    }
}

static int move_messages_between_lists(SINGLYLINKEDLIST_HANDLE from_list, SINGLYLINKEDLIST_HANDLE to_list)
{
    int result;
    LIST_ITEM_HANDLE list_item;

    result = RESULT_OK;

    while ((list_item = singlylinkedlist_get_head_item(from_list)) != NULL)
    {
        MESSAGE_QUEUE_ITEM* mq_item = (MESSAGE_QUEUE_ITEM*)singlylinkedlist_item_get_value(list_item);

        if (singlylinkedlist_remove(from_list, list_item) != 0)
        {
            LogError("failed removing message from list");
            result = MU_FAILURE;
            break;
        }
        else if (mq_item == NULL)
        {
            LogError("failed moving message to list");
            result = MU_FAILURE;
            break;
        }
        else if (singlylinkedlist_add(to_list, (const void*)mq_item) == NULL)
        {
            LogError("failed moving message to list");
            fire_message_callback(mq_item, MESSAGE_QUEUE_CANCELLED, NULL);
            free(mq_item);
            result = MU_FAILURE;
            break;
        }
    }

    return result;
}

int message_queue_move_all_back_to_pending(MESSAGE_QUEUE_HANDLE message_queue)
{
    int result;

    // Codes_SRS_MESSAGE_QUEUE_21_071: [If the message_queue is NULL, the message_queue_move_all_back_to_pending shall return non-zero result.]
    if (message_queue == NULL)
    {
        LogError("invalid argument (message_queue is NULL)");
        result = MU_FAILURE;
    }
    else
    {
        // Codes_SRS_MESSAGE_QUEUE_21_070: [The message_queue_move_all_back_to_pending shall add all in_progress message in front of the pending messages.]
        if (move_messages_between_lists(message_queue->pending, message_queue->in_progress) != 0)
        {
            LogError("failed moving pending messages at the end of in-progress");
            result = MU_FAILURE;
        }
        else if (move_messages_between_lists(message_queue->in_progress, message_queue->pending) != 0)
        {
            LogError("failed moving all in-progress messages back to pending");
            result = MU_FAILURE;
        }
        else
        {
            result = RESULT_OK;
        }

        // Codes_SRS_MESSAGE_QUEUE_21_072: [If move pending messages failed, the message_queue_move_all_back_to_pending shall delete all elements in the queue and return non-zero.]
        // Codes_SRS_MESSAGE_QUEUE_21_073: [If move in_progress messages failed, the message_queue_move_all_back_to_pending shall delete all elements in the queue and return non-zero.]
        if (result != RESULT_OK)
        {
            message_queue_remove_all(message_queue);
        }
    }

    return result;
}

void message_queue_destroy(MESSAGE_QUEUE_HANDLE message_queue)
{
    // Codes_SRS_MESSAGE_QUEUE_09_013: [If `message_queue` is NULL, message_queue_destroy shall return immediately]
    if (message_queue != NULL)
    {
        // Codes_SRS_MESSAGE_QUEUE_09_014: [message_queue_destroy shall invoke message_queue_remove_all]
        message_queue_remove_all(message_queue);

        // Codes_SRS_MESSAGE_QUEUE_09_015: [message_queue_destroy shall free all memory allocated and pointed by `message_queue`]
        if (message_queue->pending != NULL)
        {
            singlylinkedlist_destroy(message_queue->pending);
        }

        if (message_queue->in_progress != NULL)
        {
            singlylinkedlist_destroy(message_queue->in_progress);
        }

        free(message_queue);
    }
}

MESSAGE_QUEUE_HANDLE message_queue_create(MESSAGE_QUEUE_CONFIG* config)
{
    MESSAGE_QUEUE* result;

    // Codes_SRS_MESSAGE_QUEUE_09_001: [If `config` is NULL, message_queue_create shall fail and return NULL]
    if (config == NULL)
    {
        LogError("invalid configuration (NULL)");
        result = NULL;
    }
    // Codes_SRS_MESSAGE_QUEUE_09_002: [If `config->on_process_message_callback` is NULL, message_queue_create shall fail and return NULL]
    else if (config->on_process_message_callback == NULL)
    {
        LogError("invalid configuration (on_process_message_callback is NULL)");
        result = NULL;
    }
    // Codes_SRS_MESSAGE_QUEUE_09_004: [Memory shall be allocated for the MESSAGE_QUEUE data structure (aka `message_queue`)]
    else if ((result = (MESSAGE_QUEUE*)malloc(sizeof(MESSAGE_QUEUE))) == NULL)
    {
        // Codes_SRS_MESSAGE_QUEUE_09_005: [If `instance` cannot be allocated, message_queue_create shall fail and return NULL]
        LogError("failed allocating MESSAGE_QUEUE");
        result = NULL;
    }
    else
    {
        memset(result, 0, sizeof(MESSAGE_QUEUE));

        // Codes_SRS_MESSAGE_QUEUE_09_006: [`message_queue->pending` shall be set using singlylinkedlist_create()]
        if ((result->pending = singlylinkedlist_create()) == NULL)
        {
            // Codes_SRS_MESSAGE_QUEUE_09_007: [If singlylinkedlist_create fails, message_queue_create shall fail and return NULL]
            LogError("failed allocating MESSAGE_QUEUE pending list");
            // Codes_SRS_MESSAGE_QUEUE_09_011: [If any failures occur, message_queue_create shall release all memory it has allocated]
            message_queue_destroy(result);
            result = NULL;
        }
        // Codes_SRS_MESSAGE_QUEUE_09_008: [`message_queue->in_progress` shall be set using singlylinkedlist_create()]
        else if ((result->in_progress = singlylinkedlist_create()) == NULL)
        {
            // Codes_SRS_MESSAGE_QUEUE_09_009: [If singlylinkedlist_create fails, message_queue_create shall fail and return NULL]
            LogError("failed allocating MESSAGE_QUEUE in-progress list");
            // Codes_SRS_MESSAGE_QUEUE_09_011: [If any failures occur, message_queue_create shall release all memory it has allocated]
            message_queue_destroy(result);
            result = NULL;
        }
        else
        {
            // Codes_SRS_MESSAGE_QUEUE_09_010: [All arguments in `config` shall be saved into `message_queue`]
            // Codes_SRS_MESSAGE_QUEUE_09_012: [If no failures occur, message_queue_create shall return the `message_queue` pointer]

            result->max_message_enqueued_time_secs = config->max_message_enqueued_time_secs;
            result->max_message_processing_time_secs = config->max_message_processing_time_secs;
            result->max_retry_count = config->max_retry_count;
            result->on_process_message_callback = config->on_process_message_callback;
        }
    }

    return result;
}

int message_queue_add(MESSAGE_QUEUE_HANDLE message_queue, MQ_MESSAGE_HANDLE message, MESSAGE_PROCESSING_COMPLETED_CALLBACK on_message_processing_completed_callback, void* user_context)
{
    int result;

    // Codes_SRS_MESSAGE_QUEUE_09_016: [If `message_queue` or `message` are NULL, message_queue_add shall fail and return non-zero]
    if (message_queue == NULL || message == NULL)
    {
        LogError("invalid argument (message_queue=%p, message=%p)", message_queue, message);
        result = MU_FAILURE;
    }
    else
    {
        MESSAGE_QUEUE_ITEM* mq_item;

        // Codes_SRS_MESSAGE_QUEUE_09_017: [message_queue_add shall allocate a structure (aka `mq_item`) to save the `message`]
        if ((mq_item = (MESSAGE_QUEUE_ITEM*)malloc(sizeof(MESSAGE_QUEUE_ITEM))) == NULL)
        {
            // Codes_SRS_MESSAGE_QUEUE_09_018: [If `mq_item` cannot be allocated, message_queue_add shall fail and return non-zero]
            LogError("failed creating container for message");
            result = MU_FAILURE;
        }
        else
        {
            memset(mq_item, 0, sizeof(MESSAGE_QUEUE_ITEM));

            // Codes_SRS_MESSAGE_QUEUE_09_019: [`mq_item->enqueue_time` shall be set using get_time()]
            if ((mq_item->enqueue_time = get_time(NULL)) == INDEFINITE_TIME)
            {
                // Codes_SRS_MESSAGE_QUEUE_09_020: [If get_time fails, message_queue_add shall fail and return non-zero]
                LogError("failed setting message enqueue time");
                // Codes_SRS_MESSAGE_QUEUE_09_024: [If any failures occur, message_queue_add shall release all memory it has allocated]
                free(mq_item);
                result = MU_FAILURE;
            }
            // Codes_SRS_MESSAGE_QUEUE_09_021: [`mq_item` shall be added to `message_queue->pending` list]
            else if (singlylinkedlist_add(message_queue->pending, (const void*)mq_item) == NULL)
            {
                // Codes_SRS_MESSAGE_QUEUE_09_022: [`mq_item` fails to be added to `message_queue->pending`, message_queue_add shall fail and return non-zero]
                LogError("failed enqueing message");
                // Codes_SRS_MESSAGE_QUEUE_09_024: [If any failures occur, message_queue_add shall release all memory it has allocated]
                free(mq_item);
                result = MU_FAILURE;
            }
            else
            {
                // Codes_SRS_MESSAGE_QUEUE_09_023: [`message` shall be saved into `mq_item->message`]
                mq_item->message = message;
                mq_item->on_message_processing_completed_callback = on_message_processing_completed_callback;
                mq_item->user_context = user_context;
                mq_item->processing_start_time = INDEFINITE_TIME;
                // Codes_SRS_MESSAGE_QUEUE_09_025: [If no failures occur, message_queue_add shall return 0]
                result = RESULT_OK;
            }
        }
    }

    return result;
}

int message_queue_is_empty(MESSAGE_QUEUE_HANDLE message_queue, bool* is_empty)
{
    int result;

    // Codes_SRS_MESSAGE_QUEUE_09_030: [If `message_queue` or `is_empty` are NULL, message_queue_is_empty shall fail and return non-zero]
    if (message_queue == NULL || is_empty == NULL)
    {
        LogError("invalid argument (message_queue=%p, is_empty=%p)", message_queue, is_empty);
        result = MU_FAILURE;
    }
    else
    {
        // Codes_SRS_MESSAGE_QUEUE_09_031: [If `message_queue->pending` and `message_queue->in_progress` are empty, `is_empty` shall be set to true]
        // Codes_SRS_MESSAGE_QUEUE_09_032: [Otherwise `is_empty` shall be set to false]
        *is_empty = (singlylinkedlist_get_head_item(message_queue->pending) == NULL && singlylinkedlist_get_head_item(message_queue->in_progress) == NULL);
        // Codes_SRS_MESSAGE_QUEUE_09_033: [If no failures occur, message_queue_is_empty shall return 0]
        result = RESULT_OK;
    }

    return result;
}

void message_queue_do_work(MESSAGE_QUEUE_HANDLE message_queue)
{
    // Codes_SRS_MESSAGE_QUEUE_09_034: [If `message_queue` is NULL, message_queue_do_work shall return immediately]
    if (message_queue != NULL)
    {
        process_timeouts(message_queue);
        process_pending_messages(message_queue);
    }
}

int message_queue_set_max_message_enqueued_time_secs(MESSAGE_QUEUE_HANDLE message_queue, size_t seconds)
{
    int result;

    // Codes_SRS_MESSAGE_QUEUE_09_051: [If `message_queue` is NULL, message_queue_set_max_message_enqueued_time_secs shall fail and return non-zero]
    if (message_queue == NULL)
    {
        LogError("invalid argument (message_queue is NULL)");
        result = MU_FAILURE;
    }
    else
    {
        // Codes_SRS_MESSAGE_QUEUE_09_053: [`seconds` shall be saved into `message_queue->max_message_enqueued_time_secs`]
        message_queue->max_message_enqueued_time_secs = seconds;
        // Codes_SRS_MESSAGE_QUEUE_09_054: [If no failures occur, message_queue_set_max_message_enqueued_time_secs shall return 0]
        result = RESULT_OK;
    }

    return result;
}

int message_queue_set_max_message_processing_time_secs(MESSAGE_QUEUE_HANDLE message_queue, size_t seconds)
{
    int result;

    // Codes_SRS_MESSAGE_QUEUE_09_055: [If `message_queue` is NULL, message_queue_set_max_message_processing_time_secs shall fail and return non-zero]
    if (message_queue == NULL)
    {
        LogError("invalid argument (message_queue is NULL)");
        result = MU_FAILURE;
    }
    else
    {
        // Codes_SRS_MESSAGE_QUEUE_09_057: [`seconds` shall be saved into `message_queue->max_message_processing_time_secs`]
        message_queue->max_message_processing_time_secs = seconds;
        // Codes_SRS_MESSAGE_QUEUE_09_058: [If no failures occur, message_queue_set_max_message_processing_time_secs shall return 0]
        result = RESULT_OK;
    }

    return result;
}

int message_queue_set_max_retry_count(MESSAGE_QUEUE_HANDLE message_queue, size_t max_retry_count)
{
    int result;

    // Codes_SRS_MESSAGE_QUEUE_09_059: [If `message_queue` is NULL, message_queue_set_max_retry_count shall fail and return non-zero]
    if (message_queue == NULL)
    {
        LogError("invalid argument (message_queue is NULL)");
        result = MU_FAILURE;
    }
    else
    {
        // Codes_SRS_MESSAGE_QUEUE_09_061: [If no failures occur, message_queue_set_max_retry_count shall return 0]
        message_queue->max_retry_count = max_retry_count;
        // Codes_SRS_MESSAGE_QUEUE_09_060: [`max_retry_count` shall be saved into `message_queue->max_retry_count`]
        result = RESULT_OK;
    }

    return result;
}

static int setOption(void* handle, const char* name, const void* value)
{
    int result;

    if (handle == NULL || name == NULL || value == NULL)
    {
        LogError("invalid argument (handle=%p, name=%p, value=%p)", handle, name, value);
        result = MU_FAILURE;
    }
    else if (strcmp(SAVED_OPTION_MAX_ENQUEUE_TIME_SECS, name) == 0)
    {
        if (message_queue_set_max_message_enqueued_time_secs((MESSAGE_QUEUE_HANDLE)handle, *(size_t*)value) != RESULT_OK)
        {
            LogError("failed setting option %s", name);
            result = MU_FAILURE;
        }
        else
        {
            result = RESULT_OK;
        }
    }
    else if (strcmp(SAVED_OPTION_MAX_PROCESSING_TIME_SECS, name) == 0)
    {
        if (message_queue_set_max_message_processing_time_secs((MESSAGE_QUEUE_HANDLE)handle, *(size_t*)value) != RESULT_OK)
        {
            LogError("failed setting option %s", name);
            result = MU_FAILURE;
        }
        else
        {
            result = RESULT_OK;
        }
    }
    else if (strcmp(SAVED_OPTION_MAX_RETRY_COUNT, name) == 0)
    {
        if (message_queue_set_max_retry_count((MESSAGE_QUEUE_HANDLE)handle, *(size_t*)value) != RESULT_OK)
        {
            LogError("failed setting option %s", name);
            result = MU_FAILURE;
        }
        else
        {
            result = RESULT_OK;
        }
    }
    else
    {
        LogError("option %s is invalid", name);
        result = MU_FAILURE;
    }

    return result;
}

OPTIONHANDLER_HANDLE message_queue_retrieve_options(MESSAGE_QUEUE_HANDLE message_queue)
{
    OPTIONHANDLER_HANDLE result;

    // Codes_SRS_MESSAGE_QUEUE_09_062: [If `message_queue` is NULL, message_queue_retrieve_options shall fail and return NULL]
    if (message_queue == NULL)
    {
        LogError("invalid argument (message_queue is NULL)");
        result = NULL;
    }
    // Codes_SRS_MESSAGE_QUEUE_09_063: [An OPTIONHANDLER_HANDLE instance shall be created using OptionHandler_Create]
    else if ((result = OptionHandler_Create(cloneOption, destroyOption, setOption)) == NULL)
    {
        // Codes_SRS_MESSAGE_QUEUE_09_064: [If an OPTIONHANDLER_HANDLE instance fails to be created, message_queue_retrieve_options shall fail and return NULL]
        LogError("failed creating OPTIONHANDLER_HANDLE");
    }
    // Codes_SRS_MESSAGE_QUEUE_09_065: [Each option of `instance` shall be added to the OPTIONHANDLER_HANDLE instance using OptionHandler_AddOption]
    else if (OptionHandler_AddOption(result, SAVED_OPTION_MAX_ENQUEUE_TIME_SECS, &message_queue->max_message_enqueued_time_secs) != OPTIONHANDLER_OK)
    {
        LogError("failed retrieving options (failed adding %s)", SAVED_OPTION_MAX_ENQUEUE_TIME_SECS);
        // Codes_SRS_MESSAGE_QUEUE_09_067: [If message_queue_retrieve_options fails, any allocated memory shall be freed]
        OptionHandler_Destroy(result);
        // Codes_SRS_MESSAGE_QUEUE_09_066: [If OptionHandler_AddOption fails, message_queue_retrieve_options shall fail and return NULL]
        result = NULL;
    }
    else if (OptionHandler_AddOption(result, SAVED_OPTION_MAX_PROCESSING_TIME_SECS, &message_queue->max_message_processing_time_secs) != OPTIONHANDLER_OK)
    {
        LogError("failed retrieving options (failed adding %s)", SAVED_OPTION_MAX_PROCESSING_TIME_SECS);
        // Codes_SRS_MESSAGE_QUEUE_09_067: [If message_queue_retrieve_options fails, any allocated memory shall be freed]
        OptionHandler_Destroy(result);
        // Codes_SRS_MESSAGE_QUEUE_09_066: [If OptionHandler_AddOption fails, message_queue_retrieve_options shall fail and return NULL]
        result = NULL;
    }
    else if (OptionHandler_AddOption(result, SAVED_OPTION_MAX_RETRY_COUNT, &message_queue->max_retry_count) != OPTIONHANDLER_OK)
    {
        LogError("failed retrieving options (failed adding %s)", SAVED_OPTION_MAX_PROCESSING_TIME_SECS);
        // Codes_SRS_MESSAGE_QUEUE_09_067: [If message_queue_retrieve_options fails, any allocated memory shall be freed]
        OptionHandler_Destroy(result);
        // Codes_SRS_MESSAGE_QUEUE_09_066: [If OptionHandler_AddOption fails, message_queue_retrieve_options shall fail and return NULL]
        result = NULL;
    }

    // Codes_SRS_MESSAGE_QUEUE_09_068: [If no failures occur, message_queue_retrieve_options shall return the OPTIONHANDLER_HANDLE instance]
    return result;
}
