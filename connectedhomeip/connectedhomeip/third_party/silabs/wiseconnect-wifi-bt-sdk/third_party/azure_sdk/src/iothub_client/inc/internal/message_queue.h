// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

/** @file    message_queue.h
*    @brief    A generic message queue.
*/

#ifndef MESSAGE_QUEUE_H
#define MESSAGE_QUEUE_H

#include "azure_macro_utils/macro_utils.h"
#include "umock_c/umock_c_prod.h"
#include "azure_c_shared_utility/optionhandler.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct MESSAGE_QUEUE_TAG* MESSAGE_QUEUE_HANDLE;
typedef void* MQ_MESSAGE_HANDLE;
typedef void* USER_DEFINED_REASON;

#define MESSAGE_QUEUE_RESULT_STRINGS  \
    MESSAGE_QUEUE_SUCCESS,            \
    MESSAGE_QUEUE_ERROR,              \
    MESSAGE_QUEUE_RETRYABLE_ERROR,    \
    MESSAGE_QUEUE_TIMEOUT,            \
    MESSAGE_QUEUE_CANCELLED

MU_DEFINE_ENUM_WITHOUT_INVALID(MESSAGE_QUEUE_RESULT, MESSAGE_QUEUE_RESULT_STRINGS);

/**
* @brief    User-provided callback invoked by MESSAGE_QUEUE back to the user when a messages completes being processed.
*/
typedef void(*MESSAGE_PROCESSING_COMPLETED_CALLBACK)(MQ_MESSAGE_HANDLE message, MESSAGE_QUEUE_RESULT result, USER_DEFINED_REASON reason, void* user_context);

/**
* @brief    Callback that MUST be invoked by PROCESS_MESSAGE_CALLBACK (user provided) to signal to MESSAGE_QUEUE that a message has been processed.
* @remarks  Besides causing MESSAGE_QUEUE to dequeue the message from its internal lists, causes MESSAGE_PROCESSING_COMPLETED_CALLBACK to be triggered.
*/
typedef void(*PROCESS_MESSAGE_COMPLETED_CALLBACK)(MESSAGE_QUEUE_HANDLE message_queue, MQ_MESSAGE_HANDLE message, MESSAGE_QUEUE_RESULT result, USER_DEFINED_REASON reason);

/**
* @brief    User-provided callback invoked by MESSAGE_QUEUE when a messages is ready to be processed, getting internally moved from "pending" to "in-progress".
*/
typedef void(*PROCESS_MESSAGE_CALLBACK)(MESSAGE_QUEUE_HANDLE message_queue, MQ_MESSAGE_HANDLE message, PROCESS_MESSAGE_COMPLETED_CALLBACK on_process_message_completed_callback, void* user_context);

typedef struct MESSAGE_QUEUE_CONFIG_TAG
{
    /**
    * @brief    Function that actually process (a.k.a, e.g, sends) a message previously queued.
    *
    * @remarks  When MESSAGE_QUEUE is summoned to invoke @c on_process_message_callback (upon call to message_queue_do_work, when a message is moved from the pending to in-progress list),
    *           it passes as arguments the MESSAGE_QUEUE handle and a callback function that MUST be invoked by @c on_process_message_callback once it completes.
    *           The @c user_context passed is the same provided as argument by the upper layer on @c message_queue_add.
    */
    PROCESS_MESSAGE_CALLBACK on_process_message_callback;
    size_t max_message_enqueued_time_secs;
    size_t max_message_processing_time_secs;
    size_t max_retry_count;
} MESSAGE_QUEUE_CONFIG;

/**
* @brief    Creates a new instance of MESSAGE_QUEUE.
*
* @param    config    Pointer to an @c MESSAGE_QUEUE_CONFIG structure
*
* @returns    A non-NULL @c MESSAGE_QUEUE_HANDLE value that is used when invoking other API functions.
*/
MOCKABLE_FUNCTION(, MESSAGE_QUEUE_HANDLE,  message_queue_create, MESSAGE_QUEUE_CONFIG*, config);

/**
* @brief    Destroys an instance of MESSAGE_QUEUE, releasing all memory it allocated.
*
* @remarks  All messages still pending to be processed and currently in-progress get bubbled up back to the upper-layer
*           through the @c on_message_processing_completed_callback (passed on the @c MESSAGE_QUEUE_CONFIG instance)
*           with the @c result set as @c MESSAGE_QUEUE_CANCELLED and @c reason set to @c NULL.
*
* @param    message_queue    A @c MESSAGE_QUEUE_HANDLE obtained using message_queue_create.
*/
MOCKABLE_FUNCTION(, void, message_queue_destroy, MESSAGE_QUEUE_HANDLE, message_queue);

/**
* @brief    Adds a new generic message to MESSAGE_QUEUE's pending list.
*
* @param    message_queue    A @c MESSAGE_QUEUE_HANDLE obtained using message_queue_create.
*
* @param    message A generic message to be queued and then processed (i.e., sent, consolidated, etc).
*
* @returns    Zero if the no errors occur, non-zero otherwise.
*/
MOCKABLE_FUNCTION(, int, message_queue_add, MESSAGE_QUEUE_HANDLE, message_queue, MQ_MESSAGE_HANDLE, message, MESSAGE_PROCESSING_COMPLETED_CALLBACK, on_message_processing_completed_callback, void*, user_context);

/**
* @brief    Causes all messages in-progress to be moved back to the beginning of the pending list.
*
* @remarks    If on_message_process_completed_callback is invoked for any of message not in in-progress, it is disregarded.
*           Messages are queued back into the pending list in a way they will be sent first when message_queue_do_work is invoked again.
*
* @param    message_queue    A @c MESSAGE_QUEUE_HANDLE obtained using message_queue_create.
*
* @returns    Zero if the no errors occur, non-zero otherwise.
*/
MOCKABLE_FUNCTION(, int, message_queue_move_all_back_to_pending, MESSAGE_QUEUE_HANDLE, message_queue);

/**
* @brief    Causes all messages pending to be sent and in-progress to be flushed back to the user through @c on_message_processing_completed_callback.
*
* @remarks    @c on_message_processing_completed_callback gets invoked with @c result set as MESSAGE_QUEUE_CANCELLED and @c reason set to NULL.
*
* @param    message_queue    A @c MESSAGE_QUEUE_HANDLE obtained using message_queue_create.
*/
MOCKABLE_FUNCTION(, void, message_queue_remove_all, MESSAGE_QUEUE_HANDLE, message_queue);

/**
* @brief    Informs if there are messages pending to be sent and/or currently in-progress.
*
* @param    message_queue    A @c MESSAGE_QUEUE_HANDLE obtained using message_queue_create.
*
* @param    @c is_empty Set to @c true if there are any messages in pending to be sent and/or currently in-progress, @c false otherwise.
*
* @remarks    The parameter @c is_empty is only set if no errors occur (like passing a NULL @c message_queue).
*
* @returns    Zero if the no errors occur, non-zero otherwise.
*/
MOCKABLE_FUNCTION(, int, message_queue_is_empty, MESSAGE_QUEUE_HANDLE, message_queue, bool*, is_empty);

/**
* @brief    Causes MESSAGE_QUEUE to go through its list of pending messages and move them to in-progress, as well as trigering retry and timeout controls.
*
* @param    message_queue    A @c MESSAGE_QUEUE_HANDLE obtained using message_queue_create.
*/
MOCKABLE_FUNCTION(, void, message_queue_do_work, MESSAGE_QUEUE_HANDLE, message_queue);

/**
* @brief    Sets the maximum time, in seconds, a message will be within MESSAGE_QUEUE (in either pending or in-progress lists).
*
* @param    message_queue    A @c MESSAGE_QUEUE_HANDLE obtained using message_queue_create.
*
* @param    seconds    Number of seconds to set for this timeout. A value of zero de-activates this timeout control.
*
* @returns    Zero if the no errors occur, non-zero otherwise.
*/
MOCKABLE_FUNCTION(, int, message_queue_set_max_message_enqueued_time_secs, MESSAGE_QUEUE_HANDLE, message_queue, size_t, seconds);

/**
* @brief    Sets the maximum time, in seconds, a message will be in-progress within MESSAGE_QUEUE.
*
* @param    message_queue    A @c MESSAGE_QUEUE_HANDLE obtained using message_queue_create.
*
* @param    seconds    Number of seconds to set for this timeout. A value of zero de-activates this timeout control.
*
* @returns    Zero if the no errors occur, non-zero otherwise.
*/
MOCKABLE_FUNCTION(, int, message_queue_set_max_message_processing_time_secs, MESSAGE_QUEUE_HANDLE, message_queue, size_t, seconds);

/**
* @brief    Sets the maximum number of times MESSAGE_QUEUE will try to re-process a message (no counting the initial attempt).
*
* @param    message_queue    A @c MESSAGE_QUEUE_HANDLE obtained using message_queue_create.
*
* @param    max_retry_count    The number of times MESSAGE_QUEUE will try to re-process a message.
*
* @returns    Zero if the no errors occur, non-zero otherwise.
*/
MOCKABLE_FUNCTION(, int, message_queue_set_max_retry_count, MESSAGE_QUEUE_HANDLE, message_queue, size_t, max_retry_count);

/**
* @brief    Retrieves a blob with all the options currently set in the instance of MESSAGE_QUEUE.
*
* @param    message_queue    A @c MESSAGE_QUEUE_HANDLE obtained using message_queue_create.
*
* @returns    A non-NULL @c OPTIONHANDLER_HANDLE if no errors occur, or NULL otherwise.
*/
MOCKABLE_FUNCTION(, OPTIONHANDLER_HANDLE, message_queue_retrieve_options, MESSAGE_QUEUE_HANDLE, message_queue);

#ifdef __cplusplus
}
#endif

#endif /*MESSAGE_QUEUE_H*/
