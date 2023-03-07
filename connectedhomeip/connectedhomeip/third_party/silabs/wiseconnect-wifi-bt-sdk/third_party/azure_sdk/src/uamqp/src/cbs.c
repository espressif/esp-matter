// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "azure_macro_utils/macro_utils.h"
#include "azure_c_shared_utility/gballoc.h"
#include "azure_c_shared_utility/singlylinkedlist.h"
#include "azure_c_shared_utility/xlogging.h"
#include "azure_uamqp_c/cbs.h"
#include "azure_uamqp_c/amqp_management.h"
#include "azure_uamqp_c/session.h"

typedef enum CBS_STATE_TAG
{
    CBS_STATE_CLOSED,
    CBS_STATE_OPENING,
    CBS_STATE_OPEN,
    CBS_STATE_ERROR
} CBS_STATE;

typedef struct CBS_OPERATION_TAG
{
    ON_CBS_OPERATION_COMPLETE on_cbs_operation_complete;
    void* on_cbs_operation_complete_context;
    SINGLYLINKEDLIST_HANDLE pending_operations;
} CBS_OPERATION;

typedef struct CBS_INSTANCE_TAG
{
    AMQP_MANAGEMENT_HANDLE amqp_management;
    CBS_STATE cbs_state;
    ON_CBS_OPEN_COMPLETE on_cbs_open_complete;
    void* on_cbs_open_complete_context;
    ON_CBS_ERROR on_cbs_error;
    void* on_cbs_error_context;
    SINGLYLINKEDLIST_HANDLE pending_operations;
} CBS_INSTANCE;

static int add_string_key_value_pair_to_map(AMQP_VALUE map, const char* key, const char* value)
{
    int result;

    AMQP_VALUE key_value = amqpvalue_create_string(key);
    if (key_value == NULL)
    {
        /* Codes_SRS_CBS_01_072: [ If constructing the message fails, `cbs_put_token_async` shall fail and return a non-zero value. ]*/
        LogError("Failed creating value for property key %s", key);
        result = MU_FAILURE;
    }
    else
    {
        AMQP_VALUE value_value = amqpvalue_create_string(value);
        if (value_value == NULL)
        {
            /* Codes_SRS_CBS_01_072: [ If constructing the message fails, `cbs_put_token_async` shall fail and return a non-zero value. ]*/
            LogError("Failed creating value for property value %s", value);
            result = MU_FAILURE;
        }
        else
        {
            if (amqpvalue_set_map_value(map, key_value, value_value) != 0)
            {
                /* Codes_SRS_CBS_01_072: [ If constructing the message fails, `cbs_put_token_async` shall fail and return a non-zero value. ]*/
                LogError("Failed inserting key/value pair in the map");
                result = MU_FAILURE;
            }
            else
            {
                result = 0;
            }

            amqpvalue_destroy(value_value);
        }

        amqpvalue_destroy(key_value);
    }

    return result;
}

static void on_underlying_amqp_management_open_complete(void* context, AMQP_MANAGEMENT_OPEN_RESULT open_result)
{
    if (context == NULL)
    {
        /* Codes_SRS_CBS_01_105: [ When `on_amqp_management_open_complete` is called with NULL `context`, it shall do nothing. ]*/
        LogError("on_underlying_amqp_management_open_complete called with NULL context");
    }
    else
    {
        CBS_HANDLE cbs = (CBS_HANDLE)context;

        switch (cbs->cbs_state)
        {
        default:
            LogError("AMQP management open complete in unknown state");
            break;

        case CBS_STATE_CLOSED:
        case CBS_STATE_ERROR:
            LogError("Unexpected AMQP management open complete");
            break;

        case CBS_STATE_OPEN:
            LogError("Unexpected AMQP management open complete in OPEN");
            /* Codes_SRS_CBS_01_109: [ When `on_amqp_management_open_complete` is called when the CBS is OPEN, the callback `on_cbs_error` shall be called and the `on_cbs_error_context` shall be passed as argument. ]*/
            cbs->cbs_state = CBS_STATE_ERROR;
            cbs->on_cbs_error(cbs->on_cbs_error_context);
            break;

        case CBS_STATE_OPENING:
        {
            switch (open_result)
            {
            default:
                LogError("Unknown AMQP management state");

            case AMQP_MANAGEMENT_OPEN_ERROR:
                cbs->cbs_state = CBS_STATE_CLOSED;
                /* Codes_SRS_CBS_01_113: [ When `on_amqp_management_open_complete` reports a failure, the underlying AMQP management shall be closed by calling `amqp_management_close`. ]*/
                (void)amqp_management_close(cbs->amqp_management);
                /* Codes_SRS_CBS_01_107: [ If CBS is OPENING and `open_result` is `AMQP_MANAGEMENT_OPEN_ERROR` the callback `on_cbs_open_complete` shall be called with `CBS_OPEN_ERROR` and the `on_cbs_open_complete_context` shall be passed as argument. ]*/
                cbs->on_cbs_open_complete(cbs->on_cbs_open_complete_context, CBS_OPEN_ERROR);
                break;

            case AMQP_MANAGEMENT_OPEN_CANCELLED:
                cbs->cbs_state = CBS_STATE_CLOSED;
                /* Codes_SRS_CBS_01_113: [ When `on_amqp_management_open_complete` reports a failure, the underlying AMQP management shall be closed by calling `amqp_management_close`. ]*/
                (void)amqp_management_close(cbs->amqp_management);
                /* Codes_SRS_CBS_01_108: [ If CBS is OPENING and `open_result` is `AMQP_MANAGEMENT_OPEN_CANCELLED` the callback `on_cbs_open_complete` shall be called with `CBS_OPEN_CANCELLED` and the `on_cbs_open_complete_context` shall be passed as argument. ]*/
                cbs->on_cbs_open_complete(cbs->on_cbs_open_complete_context, CBS_OPEN_CANCELLED);
                break;

            case AMQP_MANAGEMENT_OPEN_OK:
                /* Codes_SRS_CBS_01_106: [ If CBS is OPENING and `open_result` is `AMQP_MANAGEMENT_OPEN_OK` the callback `on_cbs_open_complete` shall be called with `CBS_OPEN_OK` and the `on_cbs_open_complete_context` shall be passed as argument. ]*/
                cbs->cbs_state = CBS_STATE_OPEN;
                cbs->on_cbs_open_complete(cbs->on_cbs_open_complete_context, CBS_OPEN_OK);
                break;
            }
            break;
        }
        }
    }
}

static void on_underlying_amqp_management_error(void* context)
{
    if (context == NULL)
    {
        /* Codes_SRS_CBS_01_110: [ When `on_amqp_management_error` is called with NULL `context`, it shall do nothing. ]*/
        LogError("on_underlying_amqp_management_error called with NULL context");
    }
    else
    {
        CBS_HANDLE cbs = (CBS_HANDLE)context;

        switch (cbs->cbs_state)
        {
        default:
            LogError("AMQP management error in unknown state");
            break;

        case CBS_STATE_CLOSED:
            LogError("Unexpected AMQP error in CLOSED state");
            break;

        case CBS_STATE_OPENING:
            cbs->cbs_state = CBS_STATE_CLOSED;
            /* Codes_SRS_CBS_01_114: [ Additionally the underlying AMQP management shall be closed by calling `amqp_management_close`. ]*/
            (void)amqp_management_close(cbs->amqp_management);
            /* Codes_SRS_CBS_01_111: [ If CBS is OPENING the callback `on_cbs_open_complete` shall be called with `CBS_OPEN_ERROR` and the `on_cbs_open_complete_context` shall be passed as argument. ]*/
            cbs->on_cbs_open_complete(cbs->on_cbs_open_complete_context, CBS_OPEN_ERROR);
            break;

        case CBS_STATE_OPEN:
            /* Codes_SRS_CBS_01_112: [ If CBS is OPEN the callback `on_cbs_error` shall be called and the `on_cbs_error_context` shall be passed as argument. ]*/
            cbs->cbs_state = CBS_STATE_ERROR;
            cbs->on_cbs_error(cbs->on_cbs_error_context);
            break;
        }
    }
}

static void on_amqp_management_execute_operation_complete(void* context, AMQP_MANAGEMENT_EXECUTE_OPERATION_RESULT execute_operation_result, unsigned int status_code, const char* status_description, MESSAGE_HANDLE message)
{
    if (context == NULL)
    {
        /* Codes_SRS_CBS_01_091: [ When `on_amqp_management_execute_operation_complete` is called with a NULL context it shall do nothing. ]*/
        LogError("on_amqp_management_execute_operation_complete called with NULL context");
    }
    else
    {
        /* Codes_SRS_CBS_01_103: [ The `context` shall be used to obtain the pending operation information stored in the pending operations linked list by calling `singlylinkedlist_item_get_value`. ]*/
        CBS_OPERATION* cbs_operation = (CBS_OPERATION*)singlylinkedlist_item_get_value((LIST_ITEM_HANDLE)context);
        CBS_OPERATION_RESULT cbs_operation_result;

        (void)message;

        if (cbs_operation == NULL)
        {
            LogError("NULL cbs_operation");
        }
        else
        {
            switch (execute_operation_result)
            {
            default:
                cbs_operation_result = CBS_OPERATION_RESULT_CBS_ERROR;
                break;

            case AMQP_MANAGEMENT_EXECUTE_OPERATION_FAILED_BAD_STATUS:
                /* Tests_SRS_CBS_01_094: [ When `on_amqp_management_execute_operation_complete` is called with `AMQP_MANAGEMENT_EXECUTE_OPERATION_FAILED_BAD_STATUS`, the associated cbs operation complete callback shall be called with `CBS_OPERATION_RESULT_OPERATION_FAILED` and passing the `on_cbs_put_token_complete_context` as the context argument. ]*/
                cbs_operation_result = CBS_OPERATION_RESULT_OPERATION_FAILED;
                break;

            case AMQP_MANAGEMENT_EXECUTE_OPERATION_INSTANCE_CLOSED:
                /* Tests_SRS_CBS_01_115: [ When `on_amqp_management_execute_operation_complete` is called with `AMQP_MANAGEMENT_EXECUTE_OPERATION_INSTANCE_CLOSED`, the associated cbs operation complete callback shall be called with `CBS_OPERATION_RESULT_INSTANCE_CLOSED` and passing the `on_cbs_put_token_complete_context` as the context argument. ]*/
                cbs_operation_result = CBS_OPERATION_RESULT_INSTANCE_CLOSED;
                break;

            case AMQP_MANAGEMENT_EXECUTE_OPERATION_ERROR:
                /* Tests_SRS_CBS_01_093: [ When `on_amqp_management_execute_operation_complete` is called with `AMQP_MANAGEMENT_EXECUTE_OPERATION_ERROR`, the associated cbs operation complete callback shall be called with `CBS_OPERATION_RESULT_CBS_ERROR` and passing the `on_cbs_put_token_complete_context` as the context argument. ]*/
                cbs_operation_result = CBS_OPERATION_RESULT_CBS_ERROR;
                break;

            case AMQP_MANAGEMENT_EXECUTE_OPERATION_OK:
                /* Codes_SRS_CBS_01_092: [ When `on_amqp_management_execute_operation_complete` is called with `AMQP_MANAGEMENT_EXECUTE_OPERATION_OK`, the associated cbs operation complete callback shall be called with `CBS_OPERATION_RESULT_OK` and passing the `on_cbs_put_token_complete_context` as the context argument. ]*/
                cbs_operation_result = CBS_OPERATION_RESULT_OK;
                break;
            }

            /* Codes_SRS_CBS_01_095: [ `status_code` and `status_description` shall be passed as they are to the cbs operation complete callback. ]*/
            /* Codes_SRS_CBS_01_014: [ The response message has the following application-properties: ]*/
            /* Codes_SRS_CBS_01_013: [ status-code    No    int    HTTP response code [RFC2616]. ]*/
            /* Codes_SRS_CBS_01_015: [ status-description    Yes    string    Description of the status. ]*/
            /* Codes_SRS_CBS_01_016: [ The body of the message MUST be empty. ]*/
            /* Codes_SRS_CBS_01_026: [ The response message has the following application-properties: ]*/
            /* Codes_SRS_CBS_01_027: [ status-code    Yes    int    HTTP response code [RFC2616]. ]*/
            /* Codes_SRS_CBS_01_028: [ status-description    No    string    Description of the status. ]*/
            /* Codes_SRS_CBS_01_029: [ The body of the message MUST be empty. ]*/
            cbs_operation->on_cbs_operation_complete(cbs_operation->on_cbs_operation_complete_context, cbs_operation_result, status_code, status_description);

            /* Codes_SRS_CBS_01_102: [ The pending operation shall be removed from the pending operations list by calling `singlylinkedlist_remove`. ]*/
            if (singlylinkedlist_remove(cbs_operation->pending_operations, (LIST_ITEM_HANDLE)context) != 0)
            {
                LogError("Failed removing operation from the pending list");
            }

            /* Codes_SRS_CBS_01_096: [ The `context` for the operation shall also be freed. ]*/
            free(cbs_operation);
        }
    }
}

CBS_HANDLE cbs_create(SESSION_HANDLE session)
{
    CBS_INSTANCE* cbs;

    if (session == NULL)
    {
        /* Codes_SRS_CBS_01_033: [** If `session` is NULL then `cbs_create` shall fail and return NULL. ]*/
        LogError("NULL session handle");
        cbs = NULL;
    }
    else
    {
        cbs = (CBS_INSTANCE*)calloc(1, sizeof(CBS_INSTANCE));
        if (cbs == NULL)
        {
            /* Codes_SRS_CBS_01_076: [ If allocating memory for the new handle fails, `cbs_create` shall fail and return NULL. ]*/
            LogError("Cannot allocate memory for cbs instance.");
        }
        else
        {
            /* Codes_SRS_CBS_01_097: [ `cbs_create` shall create a singly linked list for pending operations by calling `singlylinkedlist_create`. ]*/
            cbs->pending_operations = singlylinkedlist_create();
            if (cbs->pending_operations == NULL)
            {
                /* Codes_SRS_CBS_01_101: [ If `singlylinkedlist_create` fails, `cbs_create` shall fail and return NULL. ]*/
                LogError("Cannot allocate pending operations list.");
            }
            else
            {
                /* Codes_SRS_CBS_01_034: [ `cbs_create` shall create an AMQP management handle by calling `amqp_management_create`. ]*/
                /* Codes_SRS_CBS_01_002: [ Tokens are communicated between AMQP peers by sending specially-formatted AMQP messages to the Claims-based Security Node. ]*/
                /* Codes_SRS_CBS_01_003: [ The mechanism follows the scheme defined in the AMQP Management specification [AMQPMAN]. ]*/
                cbs->amqp_management = amqp_management_create(session, "$cbs");
                if (cbs->amqp_management == NULL)
                {
                    LogError("Cannot create AMQP management instance for the $cbs node.");
                }
                else
                {
                    /* Codes_SRS_CBS_01_116: [ If setting the override key names fails, then `cbs_create` shall fail and return NULL. ]*/
                    if (amqp_management_set_override_status_code_key_name(cbs->amqp_management, "status-code") != 0)
                    {
                        /* Codes_SRS_CBS_01_116: [ If setting the override key names fails, then `cbs_create` shall fail and return NULL. ]*/
                        LogError("Cannot set the override status code key name");
                    }
                    else
                    {
                        /* Codes_SRS_CBS_01_118: [ `cbs_create` shall set the override status description key name on the AMQP management handle to `status-description` by calling `amqp_management_set_override_status_description_key_name`. ]*/
                        if (amqp_management_set_override_status_description_key_name(cbs->amqp_management, "status-description") != 0)
                        {
                            /* Codes_SRS_CBS_01_116: [ If setting the override key names fails, then `cbs_create` shall fail and return NULL. ]*/
                            LogError("Cannot set the override status description key name");
                        }
                        else
                        {
                            /* Codes_SRS_CBS_01_001: [ `cbs_create` shall create a new CBS instance and on success return a non-NULL handle to it. ]*/
                            cbs->cbs_state = CBS_STATE_CLOSED;

                            goto all_ok;
                        }
                    }

                    amqp_management_destroy(cbs->amqp_management);
                }

                singlylinkedlist_destroy(cbs->pending_operations);
            }

            free(cbs);
            cbs = NULL;
        }
    }

all_ok:
    return cbs;
}

void cbs_destroy(CBS_HANDLE cbs)
{
    if (cbs == NULL)
    {
        /* Codes_SRS_CBS_01_037: [ If `cbs` is NULL, `cbs_destroy` shall do nothing. ]*/
        LogError("NULL cbs handle");
    }
    else
    {
        LIST_ITEM_HANDLE first_pending_operation;

        /* Codes_SRS_CBS_01_100: [ If the CBS instance is not closed, all actions performed by `cbs_close` shall be performed. ]*/
        if (cbs->cbs_state != CBS_STATE_CLOSED)
        {
            (void)amqp_management_close(cbs->amqp_management);
        }

        /* Codes_SRS_CBS_01_036: [ `cbs_destroy` shall free all resources associated with the handle `cbs`. ]*/
        /* Codes_SRS_CBS_01_038: [ `cbs_destroy` shall free the AMQP management handle created in `cbs_create` by calling `amqp_management_destroy`. ]*/
        amqp_management_destroy(cbs->amqp_management);

        /* Codes_SRS_CBS_01_099: [ All pending operations shall be freed. ]*/
        while ((first_pending_operation = singlylinkedlist_get_head_item(cbs->pending_operations)) != NULL)
        {
            CBS_OPERATION* pending_operation = (CBS_OPERATION*)singlylinkedlist_item_get_value(first_pending_operation);
            if (pending_operation != NULL)
            {
                pending_operation->on_cbs_operation_complete(pending_operation->on_cbs_operation_complete_context, CBS_OPERATION_RESULT_INSTANCE_CLOSED, 0, NULL);
                free(pending_operation);
            }

            singlylinkedlist_remove(cbs->pending_operations, first_pending_operation);
        }

        /* Codes_SRS_CBS_01_098: [ `cbs_destroy` shall free the pending operations list by calling `singlylinkedlist_destroy`. ]*/
        singlylinkedlist_destroy(cbs->pending_operations);
        free(cbs);
    }
}

int cbs_open_async(CBS_HANDLE cbs, ON_CBS_OPEN_COMPLETE on_cbs_open_complete, void* on_cbs_open_complete_context, ON_CBS_ERROR on_cbs_error, void* on_cbs_error_context)
{
    int result;

    /* Codes_SRS_CBS_01_042: [ `on_cbs_open_complete_context` and `on_cbs_error_context` shall be allowed to be NULL. ]*/
    if ((cbs == NULL) ||
        (on_cbs_open_complete == NULL) ||
        (on_cbs_error == NULL))
    {
        /* Codes_SRS_CBS_01_040: [ If any of the arguments `cbs`, `on_cbs_open_complete` or `on_cbs_error` is NULL, then `cbs_open_async` shall fail and return a non-zero value. ]*/
        LogError("Bad arguments: cbs = %p, on_cbs_open_complete = %p, on_cbs_error = %p",
            cbs, on_cbs_open_complete, on_cbs_error);
        result = MU_FAILURE;
    }
    else if (cbs->cbs_state != CBS_STATE_CLOSED)
    {
        /* Codes_SRS_CBS_01_043: [ `cbs_open_async` while already open or opening shall fail and return a non-zero value. ]*/
        LogError("cbs instance already open");
        result = MU_FAILURE;
    }
    else
    {
        /* Codes_SRS_CBS_01_078: [ The cbs instance shall be considered OPENING until the `on_amqp_management_open_complete` callback is called by the AMQP management instance indicating that the open has completed (succesfull or not). ]*/
        cbs->cbs_state = CBS_STATE_OPENING;
        cbs->on_cbs_open_complete = on_cbs_open_complete;
        cbs->on_cbs_open_complete_context = on_cbs_open_complete_context;
        cbs->on_cbs_error = on_cbs_error;
        cbs->on_cbs_error_context = on_cbs_error_context;

        /* Codes_SRS_CBS_01_039: [ `cbs_open_async` shall open the cbs communication by calling `amqp_management_open_async` on the AMQP management handle created in `cbs_create`. ]*/
        if (amqp_management_open_async(cbs->amqp_management, on_underlying_amqp_management_open_complete, cbs, on_underlying_amqp_management_error, cbs) != 0)
        {
            /* Codes_SRS_CBS_01_041: [ If `amqp_management_open_async` fails, shall fail and return a non-zero value. ]*/
            result = MU_FAILURE;
        }
        else
        {
            /* Codes_SRS_CBS_01_077: [ On success, `cbs_open_async` shall return 0. ]*/
            result = 0;
        }
    }

    return result;
}

int cbs_close(CBS_HANDLE cbs)
{
    int result;

    if (cbs == NULL)
    {
        /* Codes_SRS_CBS_01_045: [ If the argument `cbs` is NULL, `cbs_close` shall fail and return a non-zero value. ]*/
        LogError("NULL cbs handle");
        result = MU_FAILURE;
    }
    else if (cbs->cbs_state == CBS_STATE_CLOSED)
    {
        /* Codes_SRS_CBS_01_047: [ `cbs_close` when closed shall fail and return a non-zero value. ]*/
        /* Codes_SRS_CBS_01_048: [ `cbs_close` when not opened shall fail and return a non-zero value. ]*/
        LogError("Already closed");
        result = MU_FAILURE;
    }
    else
    {
        /* Codes_SRS_CBS_01_044: [ `cbs_close` shall close the CBS instance by calling `amqp_management_close` on the underlying AMQP management handle. ]*/
        if (amqp_management_close(cbs->amqp_management) != 0)
        {
            /* Codes_SRS_CBS_01_046: [ If `amqp_management_close` fails, `cbs_close` shall fail and return a non-zero value. ]*/
            LogError("Failed closing AMQP management instance");
            result = MU_FAILURE;
        }
        else
        {
            if (cbs->cbs_state == CBS_STATE_OPENING)
            {
                /* Codes_SRS_CBS_01_079: [ If `cbs_close` is called while OPENING, the `on_cbs_open_complete` callback shall be called with `CBS_OPEN_CANCELLED`, while passing the `on_cbs_open_complete_context` as argument. ]*/
                cbs->on_cbs_open_complete(cbs->on_cbs_open_complete_context, CBS_OPEN_CANCELLED);
            }

            cbs->cbs_state = CBS_STATE_CLOSED;

            /* Codes_SRS_CBS_01_080: [ On success, `cbs_close` shall return 0. ]*/
            result = 0;
        }
    }

    return result;
}

int cbs_put_token_async(CBS_HANDLE cbs, const char* type, const char* audience, const char* token, ON_CBS_OPERATION_COMPLETE on_cbs_put_token_complete, void* on_cbs_put_token_complete_context)
{
    int result;

    /* Codes_SRS_CBS_01_083: [ `on_cbs_put_token_complete_context` shall be allowed to be NULL. ]*/
    if ((cbs == NULL) ||
        (type == NULL) ||
        (audience == NULL) ||
        (token == NULL) ||
        (on_cbs_put_token_complete == NULL))
    {
        /* Codes_SRS_CBS_01_050: [ If any of the arguments `cbs`, `type`, `audience`, `token` or `on_cbs_put_token_complete` is NULL `cbs_put_token_async` shall fail and return a non-zero value. ]*/
        LogError("Bad arguments: cbs = %p, type = %p, audience = %p, token = %p, on_cbs_put_token_complete = %p",
            cbs, type, audience, token, on_cbs_put_token_complete);
        result = MU_FAILURE;
    }
    else if ((cbs->cbs_state == CBS_STATE_CLOSED) ||
        (cbs->cbs_state == CBS_STATE_ERROR))
    {
        /* Codes_SRS_CBS_01_058: [ If `cbs_put_token_async` is called when the CBS instance is not yet open or in error, it shall fail and return a non-zero value. ]*/
        LogError("put token called while closed or in error");
        result = MU_FAILURE;
    }
    else
    {
        /* Codes_SRS_CBS_01_049: [ `cbs_put_token_async` shall construct a request message for the `put-token` operation. ]*/
        MESSAGE_HANDLE message = message_create();
        if (message == NULL)
        {
            /* Codes_SRS_CBS_01_072: [ If constructing the message fails, `cbs_put_token_async` shall fail and return a non-zero value. ]*/
            LogError("message_create failed");
            result = MU_FAILURE;
        }
        else
        {
            AMQP_VALUE token_value = amqpvalue_create_string(token);
            if (token_value == NULL)
            {
                /* Codes_SRS_CBS_01_072: [ If constructing the message fails, `cbs_put_token_async` shall fail and return a non-zero value. ]*/
                LogError("Failed creating token AMQP value");
                result = MU_FAILURE;
            }
            else
            {
                /* Codes_SRS_CBS_01_009: [ The body of the message MUST contain the token. ]*/
                if (message_set_body_amqp_value(message, token_value) != 0)
                {
                    /* Codes_SRS_CBS_01_072: [ If constructing the message fails, `cbs_put_token_async` shall fail and return a non-zero value. ]*/
                    LogError("Failed setting the token in the message body");
                    result = MU_FAILURE;
                }
                else
                {
                    AMQP_VALUE application_properties = amqpvalue_create_map();
                    if (application_properties == NULL)
                    {
                        /* Codes_SRS_CBS_01_072: [ If constructing the message fails, `cbs_put_token_async` shall fail and return a non-zero value. ]*/
                        LogError("Failed creating application properties map");
                        result = MU_FAILURE;
                    }
                    else
                    {
                        if (add_string_key_value_pair_to_map(application_properties, "name", audience) != 0)
                        {
                            result = MU_FAILURE;
                        }
                        else
                        {
                            if (message_set_application_properties(message, application_properties) != 0)
                            {
                                /* Codes_SRS_CBS_01_072: [ If constructing the message fails, `cbs_put_token_async` shall fail and return a non-zero value. ]*/
                                LogError("Failed setting message application properties");
                                result = MU_FAILURE;
                            }
                            else
                            {
                                CBS_OPERATION* cbs_operation = (CBS_OPERATION*)malloc(sizeof(CBS_OPERATION));
                                if (cbs_operation == NULL)
                                {
                                    LogError("Failed allocating CBS operation instance");
                                    result = MU_FAILURE;
                                }
                                else
                                {
                                    LIST_ITEM_HANDLE list_item;

                                    cbs_operation->on_cbs_operation_complete = on_cbs_put_token_complete;
                                    cbs_operation->on_cbs_operation_complete_context = on_cbs_put_token_complete_context;
                                    cbs_operation->pending_operations = cbs->pending_operations;

                                    list_item = singlylinkedlist_add(cbs->pending_operations, cbs_operation);
                                    if (list_item == NULL)
                                    {
                                        free(cbs_operation);
                                        LogError("Failed adding pending operation to list");
                                        result = MU_FAILURE;
                                    }
                                    else
                                    {
                                        /* Codes_SRS_CBS_01_051: [ `cbs_put_token_async` shall start the AMQP management operation by calling `amqp_management_execute_operation_async`, while passing to it: ]*/
                                        /* Codes_SRS_CBS_01_052: [ The `amqp_management` argument shall be the one for the AMQP management instance created in `cbs_create`. ]*/
                                        /* Codes_SRS_CBS_01_053: [ The `operation` argument shall be `put-token`. ]*/
                                        /* Codes_SRS_CBS_01_054: [ The `type` argument shall be set to the `type` argument. ]*/
                                        /* Codes_SRS_CBS_01_055: [ The `locales` argument shall be set to NULL. ]*/
                                        /* Codes_SRS_CBS_01_056: [ The `message` argument shall be the message constructed earlier according to the CBS spec. ]*/
                                        /* Codes_SRS_CBS_01_057: [ The arguments `on_execute_operation_complete` and `context` shall be set to a callback that is to be called by the AMQP management module when the operation is complete. ]*/
                                        /* Codes_SRS_CBS_01_005: [ operation    No    string    "put-token" ]*/
                                        /* Codes_SRS_CBS_01_006: [ Type    No    string    The type of the token being put, e.g., "amqp:jwt". ]*/
                                        /* Codes_SRS_CBS_01_007: [ name    No    string    The "audience" to which the token applies. ]*/
                                        if (amqp_management_execute_operation_async(cbs->amqp_management, "put-token", type, NULL, message, on_amqp_management_execute_operation_complete, list_item) != 0)
                                        {
                                            singlylinkedlist_remove(cbs->pending_operations, list_item);
                                            free(cbs_operation);
                                            /* Codes_SRS_CBS_01_084: [ If `amqp_management_execute_operation_async` fails `cbs_put_token_async` shall fail and return a non-zero value. ]*/
                                            LogError("Failed starting AMQP management operation");
                                            result = MU_FAILURE;
                                        }
                                        else
                                        {
                                            /* Codes_SRS_CBS_01_081: [ On success `cbs_put_token_async` shall return 0. ]*/
                                            result = 0;
                                        }
                                    }
                                }
                            }
                        }

                        amqpvalue_destroy(application_properties);
                    }

                    amqpvalue_destroy(token_value);
                }
            }

            message_destroy(message);
        }
    }

    return result;
}

int cbs_delete_token_async(CBS_HANDLE cbs, const char* type, const char* audience, ON_CBS_OPERATION_COMPLETE on_cbs_delete_token_complete, void* on_cbs_delete_token_complete_context)
{
    int result;

    /* Codes_SRS_CBS_01_086: [ `on_cbs_delete_token_complete_context` shall be allowed to be NULL. ]*/
    if ((cbs == NULL) ||
        (type == NULL) ||
        (audience == NULL) ||
        (on_cbs_delete_token_complete == NULL))
    {
        /* Codes_SRS_CBS_01_060: [ If any of the arguments `cbs`, `type`, `audience` or `on_cbs_delete_token_complete` is NULL `cbs_put_token_async` shall fail and return a non-zero value. ]*/
        LogError("Bad arguments: cbs = %p, type = %p, audience = %p, on_cbs_delete_token_complete = %p",
            cbs, type, audience, on_cbs_delete_token_complete);
        result = MU_FAILURE;
    }
    else if ((cbs->cbs_state == CBS_STATE_CLOSED) ||
        (cbs->cbs_state == CBS_STATE_ERROR))
    {
        /* Codes_SRS_CBS_01_067: [ If `cbs_delete_token_async` is called when the CBS instance is not yet open or in error, it shall fail and return a non-zero value. ]*/
        LogError("put token called while closed or in error");
        result = MU_FAILURE;
    }
    else
    {
        /* Codes_SRS_CBS_01_025: [ The body of the message MUST be empty. ]*/
        /* Codes_SRS_CBS_01_059: [ `cbs_delete_token_async` shall construct a request message for the `delete-token` operation. ]*/
        MESSAGE_HANDLE message = message_create();
        if (message == NULL)
        {
            /* Codes_SRS_CBS_01_071: [ If constructing the message fails, `cbs_delete_token_async` shall fail and return a non-zero value. ]*/
            LogError("message_create failed");
            result = MU_FAILURE;
        }
        else
        {
            AMQP_VALUE application_properties = amqpvalue_create_map();
            if (application_properties == NULL)
            {
                /* Codes_SRS_CBS_01_071: [ If constructing the message fails, `cbs_delete_token_async` shall fail and return a non-zero value. ]*/
                LogError("Failed creating application properties map");
                result = MU_FAILURE;
            }
            else
            {
                if (add_string_key_value_pair_to_map(application_properties, "name", audience) != 0)
                {
                    result = MU_FAILURE;
                }
                else
                {
                    /* Codes_SRS_CBS_01_021: [ The request message has the following application-properties: ]*/
                    if (message_set_application_properties(message, application_properties) != 0)
                    {
                        /* Codes_SRS_CBS_01_071: [ If constructing the message fails, `cbs_delete_token_async` shall fail and return a non-zero value. ]*/
                        LogError("Failed setting message application properties");
                        result = MU_FAILURE;
                    }
                    else
                    {
                        CBS_OPERATION* cbs_operation = (CBS_OPERATION*)malloc(sizeof(CBS_OPERATION));
                        if (cbs_operation == NULL)
                        {
                            LogError("Failed allocating CBS operation instance");
                            result = MU_FAILURE;
                        }
                        else
                        {
                            LIST_ITEM_HANDLE list_item;

                            cbs_operation->on_cbs_operation_complete = on_cbs_delete_token_complete;
                            cbs_operation->on_cbs_operation_complete_context = on_cbs_delete_token_complete_context;
                            cbs_operation->pending_operations = cbs->pending_operations;

                            list_item = singlylinkedlist_add(cbs->pending_operations, cbs_operation);
                            if (list_item == NULL)
                            {
                                free(cbs_operation);
                                LogError("Failed adding pending operation to list");
                                result = MU_FAILURE;
                            }
                            else
                            {
                                /* Codes_SRS_CBS_01_061: [ `cbs_delete_token_async` shall start the AMQP management operation by calling `amqp_management_execute_operation_async`, while passing to it: ]*/
                                /* Codes_SRS_CBS_01_085: [ The `amqp_management` argument shall be the one for the AMQP management instance created in `cbs_create`. ]*/
                                /* Codes_SRS_CBS_01_062: [ The `operation` argument shall be `delete-token`. ]*/
                                /* Codes_SRS_CBS_01_063: [ The `type` argument shall be set to the `type` argument. ]*/
                                /* Codes_SRS_CBS_01_064: [ The `locales` argument shall be set to NULL. ]*/
                                /* Codes_SRS_CBS_01_065: [ The `message` argument shall be the message constructed earlier according to the CBS spec. ]*/
                                /* Codes_SRS_CBS_01_066: [ The arguments `on_operation_complete` and `context` shall be set to a callback that is to be called by the AMQP management module when the operation is complete. ]*/
                                /* Codes_SRS_CBS_01_020: [ To instruct a peer to delete a token associated with a specific audience, a "delete-token" message can be sent to the CBS Node ]*/
                                /* Codes_SRS_CBS_01_022: [ operation    Yes    string    "delete-token" ]*/
                                /* Codes_SRS_CBS_01_023: [ Type    Yes    string    The type of the token being deleted, e.g., "amqp:jwt". ]*/
                                /* Codes_SRS_CBS_01_024: [ name    Yes    string    The "audience" of the token being deleted. ]*/
                                if (amqp_management_execute_operation_async(cbs->amqp_management, "delete-token", type, NULL, message, on_amqp_management_execute_operation_complete, list_item) != 0)
                                {
                                    /* Codes_SRS_CBS_01_087: [ If `amqp_management_execute_operation_async` fails `cbs_put_token_async` shall fail and return a non-zero value. ]*/
                                    singlylinkedlist_remove(cbs->pending_operations, list_item);
                                    free(cbs_operation);
                                    LogError("Failed starting AMQP management operation");
                                    result = MU_FAILURE;
                                }
                                else
                                {
                                    /* Codes_SRS_CBS_01_082: [ On success `cbs_delete_token_async` shall return 0. ]*/
                                    result = 0;
                                }
                            }
                        }
                    }
                }

                amqpvalue_destroy(application_properties);
            }

            message_destroy(message);
        }
    }
    return result;
}

int cbs_set_trace(CBS_HANDLE cbs, bool trace_on)
{
    int result;

    if (cbs == NULL)
    {
        /* Codes_SRS_CBS_01_090: [ If the argument `cbs` is NULL, `cbs_set_trace` shall fail and return a non-zero value. ]*/
        LogError("NULL cbs handle");
        result = MU_FAILURE;
    }
    else
    {
        /* Codes_SRS_CBS_01_088: [ `cbs_set_trace` shall enable or disable tracing by calling `amqp_management_set_trace` to pass down the `trace_on` value. ]*/
        amqp_management_set_trace(cbs->amqp_management, trace_on);

        /* Codes_SRS_CBS_01_089: [ On success, `cbs_set_trace` shall return 0. ]*/
        result = 0;
    }

    return result;
}
