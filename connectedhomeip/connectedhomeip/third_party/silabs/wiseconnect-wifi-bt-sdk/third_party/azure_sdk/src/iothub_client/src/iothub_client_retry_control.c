// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "internal/iothub_client_retry_control.h"

#include <math.h>

#include "azure_c_shared_utility/gballoc.h"
#include "azure_c_shared_utility/agenttime.h"
#include "azure_c_shared_utility/optimize_size.h"
#include "azure_c_shared_utility/xlogging.h"

#define RESULT_OK                 0
#define INDEFINITE_TIME           ((time_t)-1)
#define DEFAULT_MAX_DELAY_IN_SECS 30

typedef struct RETRY_CONTROL_INSTANCE_TAG
{
    IOTHUB_CLIENT_RETRY_POLICY policy;
    unsigned int max_retry_time_in_secs;

    unsigned int initial_wait_time_in_secs;
    unsigned int max_jitter_percent;
    unsigned int max_delay_in_secs;

    unsigned int retry_count;
    time_t first_retry_time;
    time_t last_retry_time;
    unsigned int current_wait_time_in_secs;
} RETRY_CONTROL_INSTANCE;

typedef int (*RETRY_ACTION_EVALUATION_FUNCTION)(RETRY_CONTROL_INSTANCE* retry_state, RETRY_ACTION* retry_action);


// ========== Helper Functions ========== //

// ---------- Set/Retrieve Options Helpers ----------//

static void* retry_control_clone_option(const char* name, const void* value)
{
    void* result;

    if ((name == NULL) || (value == NULL))
    {
        LogError("Failed to clone option (either name (%p) or value (%p) are NULL)", name, value);
        result = NULL;
    }
    else if (strcmp(RETRY_CONTROL_OPTION_INITIAL_WAIT_TIME_IN_SECS, name) == 0 ||
            strcmp(RETRY_CONTROL_OPTION_MAX_JITTER_PERCENT, name) == 0)
    {
        unsigned int* cloned_value;

        if ((cloned_value = (unsigned int*)malloc(sizeof(unsigned int))) == NULL)
        {
            LogError("Failed to clone option '%p' (malloc failed)", name);
            result = NULL;
        }
        else
        {
            *cloned_value = *(unsigned int*)value;

            result = (void*)cloned_value;
        }
    }
    else
    {
        LogError("Failed to clone option (option with name '%s' is not suppported)", name);
        result = NULL;
    }

    return result;
}

static void retry_control_destroy_option(const char* name, const void* value)
{
    if ((name == NULL) || (value == NULL))
    {
        LogError("Failed to destroy option (either name (%p) or value (%p) are NULL)", name, value);
    }
    else if (strcmp(RETRY_CONTROL_OPTION_INITIAL_WAIT_TIME_IN_SECS, name) == 0 ||
        strcmp(RETRY_CONTROL_OPTION_MAX_JITTER_PERCENT, name) == 0)
    {
        free((void*)value);
    }
    else
    {
        LogError("Failed to destroy option (option with name '%s' is not suppported)", name);
    }
}

// ========== _should_retry() Auxiliary Functions ========== //

static int evaluate_retry_action(RETRY_CONTROL_INSTANCE* retry_control, RETRY_ACTION* retry_action)
{
    int result;

    // Codes_SRS_IOTHUB_CLIENT_RETRY_CONTROL_09_019: [If `retry_control->retry_count` is 0, `retry_action` shall be set to RETRY_ACTION_RETRY_NOW]
    if (retry_control->retry_count == 0)
    {
        *retry_action = RETRY_ACTION_RETRY_NOW;
        result = RESULT_OK;
    }
    // Codes_SRS_IOTHUB_CLIENT_RETRY_CONTROL_09_020: [If `retry_control->last_retry_time` is INDEFINITE_TIME and policy is not IOTHUB_CLIENT_RETRY_IMMEDIATE, the evaluation function shall return non-zero]
    else if (retry_control->last_retry_time == INDEFINITE_TIME &&
             retry_control->policy != IOTHUB_CLIENT_RETRY_IMMEDIATE)
    {
        LogError("Failed to evaluate retry action (last_retry_time is INDEFINITE_TIME)");
        result = MU_FAILURE;
    }
    else
    {
        time_t current_time;

        // Codes_SRS_IOTHUB_CLIENT_RETRY_CONTROL_09_021: [`current_time` shall be set using get_time()]
        if ((current_time = get_time(NULL)) == INDEFINITE_TIME)
        {
            // Codes_SRS_IOTHUB_CLIENT_RETRY_CONTROL_09_022: [If get_time() fails, the evaluation function shall return non-zero]
            LogError("Failed to evaluate retry action (get_time() failed)");
            result = MU_FAILURE;
        }
        // Codes_SRS_IOTHUB_CLIENT_RETRY_CONTROL_09_023: [If `retry_control->max_retry_time_in_secs` is not 0 and (`current_time` - `retry_control->first_retry_time`) is greater than or equal to `retry_control->max_retry_time_in_secs`, `retry_action` shall be set to RETRY_ACTION_STOP_RETRYING]
        else if (retry_control->max_retry_time_in_secs > 0 &&
            get_difftime(current_time, retry_control->first_retry_time) >= retry_control->max_retry_time_in_secs)
        {
            *retry_action = RETRY_ACTION_STOP_RETRYING;

            // Codes_SRS_IOTHUB_CLIENT_RETRY_CONTROL_09_026: [If no errors occur, the evaluation function shall return 0]
            result = RESULT_OK;
        }
        // Codes_SRS_IOTHUB_CLIENT_RETRY_CONTROL_09_028: [If `retry_control->policy` is IOTHUB_CLIENT_RETRY_IMMEDIATE, retry_action shall be set to RETRY_ACTION_RETRY_NOW]
        else if (retry_control->policy == IOTHUB_CLIENT_RETRY_IMMEDIATE)
        {
            *retry_action = RETRY_ACTION_RETRY_NOW;

            // Codes_SRS_IOTHUB_CLIENT_RETRY_CONTROL_09_026: [If no errors occur, the evaluation function shall return 0]
            result = RESULT_OK;
        }
        // Codes_SRS_IOTHUB_CLIENT_RETRY_CONTROL_09_024: [Otherwise, if (`current_time` - `retry_control->last_retry_time`) is less than `retry_control->current_wait_time_in_secs`, `retry_action` shall be set to RETRY_ACTION_RETRY_LATER]
        else if (get_difftime(current_time, retry_control->last_retry_time) < retry_control->current_wait_time_in_secs)
        {
            *retry_action = RETRY_ACTION_RETRY_LATER;

            // Codes_SRS_IOTHUB_CLIENT_RETRY_CONTROL_09_026: [If no errors occur, the evaluation function shall return 0]
            result = RESULT_OK;
        }
        // Codes_SRS_IOTHUB_CLIENT_RETRY_CONTROL_09_025: [Otherwise, if (`current_time` - `retry_control->last_retry_time`) is greater or equal to `retry_control->current_wait_time_in_secs`, `retry_action` shall be set to RETRY_ACTION_RETRY_NOW]
        else
        {
            *retry_action = RETRY_ACTION_RETRY_NOW;

            // Codes_SRS_IOTHUB_CLIENT_RETRY_CONTROL_09_026: [If no errors occur, the evaluation function shall return 0]
            result = RESULT_OK;
        }
    }

    return result;
}

static unsigned int calculate_next_wait_time(RETRY_CONTROL_INSTANCE* retry_control)
{
    unsigned int result;

    // Codes_SRS_IOTHUB_CLIENT_RETRY_CONTROL_09_029: [If `retry_control->policy` is IOTHUB_CLIENT_RETRY_INTERVAL, `calculate_next_wait_time` shall return `retry_control->initial_wait_time_in_secs`]
    if (retry_control->policy == IOTHUB_CLIENT_RETRY_INTERVAL)
    {
        result = retry_control->initial_wait_time_in_secs;
    }
    // Codes_SRS_IOTHUB_CLIENT_RETRY_CONTROL_09_030: [If `retry_control->policy` is IOTHUB_CLIENT_RETRY_LINEAR_BACKOFF, `calculate_next_wait_time` shall return (`retry_control->initial_wait_time_in_secs` * (`retry_control->retry_count`))]
    else if (retry_control->policy == IOTHUB_CLIENT_RETRY_LINEAR_BACKOFF)
    {
        unsigned int base_delay = retry_control->initial_wait_time_in_secs * (retry_control->retry_count);

        if (base_delay > retry_control->max_delay_in_secs) 
        {
            base_delay = retry_control->max_delay_in_secs;
        }        
        
        result = base_delay;
    }
    // Codes_SRS_IOTHUB_CLIENT_RETRY_CONTROL_09_031: [If `retry_control->policy` is IOTHUB_CLIENT_RETRY_EXPONENTIAL_BACKOFF, `calculate_next_wait_time` shall return (pow(2, `retry_control->retry_count` - 1) * `retry_control->initial_wait_time_in_secs`)]
    else if (retry_control->policy == IOTHUB_CLIENT_RETRY_EXPONENTIAL_BACKOFF)
    {
        double base_delay = pow(2, retry_control->retry_count - 1) * retry_control->initial_wait_time_in_secs;

        if (base_delay > retry_control->max_delay_in_secs) 
        {
            base_delay = retry_control->max_delay_in_secs;
        }

        result = (unsigned int)base_delay;
    }
    // Codes_SRS_IOTHUB_CLIENT_RETRY_CONTROL_09_032: [If `retry_control->policy` is IOTHUB_CLIENT_RETRY_EXPONENTIAL_BACKOFF_WITH_JITTER, `calculate_next_wait_time` shall return ((pow(2, `retry_control->retry_count` - 1) * `retry_control->initial_wait_time_in_secs`) * (1 + (`retry_control->max_jitter_percent` / 100) * (rand() / RAND_MAX)))]
    else if (retry_control->policy == IOTHUB_CLIENT_RETRY_EXPONENTIAL_BACKOFF_WITH_JITTER)
    {
        double jitter_percent = (retry_control->max_jitter_percent / 100.0) * (rand() / ((double)RAND_MAX));

        double base_delay = pow(2, retry_control->retry_count - 1) * retry_control->initial_wait_time_in_secs;

        if (base_delay > retry_control->max_delay_in_secs) 
        {
            base_delay = retry_control->max_delay_in_secs;
        }

        result =  (unsigned int)(base_delay * (1 + jitter_percent));
    }
    // Codes_SRS_IOTHUB_CLIENT_RETRY_CONTROL_09_033: [If `retry_control->policy` is IOTHUB_CLIENT_RETRY_RANDOM, `calculate_next_wait_time` shall return (`retry_control->initial_wait_time_in_secs` * (rand() / RAND_MAX))]
    else if (retry_control->policy == IOTHUB_CLIENT_RETRY_RANDOM)
    {
        double random_percent = ((double)rand() / (double)RAND_MAX);
        result = (unsigned int)(retry_control->initial_wait_time_in_secs * random_percent);
    }
    else
    {
        LogError("Failed to calculate the next wait time (policy %d is not expected)", retry_control->policy);

        result = 0;
    }

    return result;
}


// ========== Public API ========== //

int is_timeout_reached(time_t start_time, unsigned int timeout_in_secs, bool* is_timed_out)
{
    int result;

    // Codes_SRS_IOTHUB_CLIENT_RETRY_CONTROL_09_057: [If `start_time` is INDEFINITE_TIME, `is_timeout_reached` shall fail and return non-zero]
    if (start_time == INDEFINITE_TIME)
    {
        LogError("Failed to verify timeout (start_time is INDEFINITE)");
        result = MU_FAILURE;
    }
    // Codes_SRS_IOTHUB_CLIENT_RETRY_CONTROL_09_058: [If `is_timed_out` is NULL, `is_timeout_reached` shall fail and return non-zero]
    else if (is_timed_out == NULL)
    {
        LogError("Failed to verify timeout (is_timed_out is NULL)");
        result = MU_FAILURE;
    }
    else
    {
        time_t current_time;

        // Codes_SRS_IOTHUB_CLIENT_RETRY_CONTROL_09_059: [`is_timeout_reached` shall obtain the `current_time` using get_time()]
        if ((current_time = get_time(NULL)) == INDEFINITE_TIME)
        {
            // Codes_SRS_IOTHUB_CLIENT_RETRY_CONTROL_09_060: [If get_time() fails, `is_timeout_reached` shall fail and return non-zero]
            LogError("Failed to verify timeout (get_time failed)");
            result = MU_FAILURE;
        }
        else
        {
            // Codes_SRS_IOTHUB_CLIENT_RETRY_CONTROL_09_061: [If (`current_time` - `start_time`) is greater or equal to `timeout_in_secs`, `is_timed_out` shall be set to true]
            if (get_difftime(current_time, start_time) >= timeout_in_secs)
            {
                *is_timed_out = true;
            }
            // Codes_SRS_IOTHUB_CLIENT_RETRY_CONTROL_09_062: [If (`current_time` - `start_time`) is less than `timeout_in_secs`, `is_timed_out` shall be set to false]
            else
            {
                *is_timed_out = false;
            }

            // Codes_SRS_IOTHUB_CLIENT_RETRY_CONTROL_09_063: [If no errors occur, `is_timeout_reached` shall return 0]
            result = RESULT_OK;
        }
    }

    return result;
}

void retry_control_reset(RETRY_CONTROL_HANDLE retry_control_handle)
{
    // Codes_SRS_IOTHUB_CLIENT_RETRY_CONTROL_09_034: [If `retry_control_handle` is NULL, `retry_control_reset` shall return]
    if (retry_control_handle == NULL)
    {
        LogError("Failed to reset the retry control (retry_state_handle is NULL)");
    }
    else
    {
        RETRY_CONTROL_INSTANCE* retry_control = (RETRY_CONTROL_INSTANCE*)retry_control_handle;

        // Codes_SRS_IOTHUB_CLIENT_RETRY_CONTROL_09_035: [`retry_control` shall have fields `retry_count` and `current_wait_time_in_secs` set to 0 (zero), `first_retry_time` and `last_retry_time` set to INDEFINITE_TIME]
        retry_control->retry_count = 0;
        retry_control->current_wait_time_in_secs = 0;
        retry_control->first_retry_time = INDEFINITE_TIME;
        retry_control->last_retry_time = INDEFINITE_TIME;
    }
}

RETRY_CONTROL_HANDLE retry_control_create(IOTHUB_CLIENT_RETRY_POLICY policy, unsigned int max_retry_time_in_secs)
{
    RETRY_CONTROL_INSTANCE* retry_control;

    // Codes_SRS_IOTHUB_CLIENT_RETRY_CONTROL_09_002: [`retry_control_create` shall allocate memory for the retry control instance structure (a.k.a. `retry_control`)]
    if ((retry_control = (RETRY_CONTROL_INSTANCE*)malloc(sizeof(RETRY_CONTROL_INSTANCE))) == NULL)
    {
        // Codes_SRS_IOTHUB_CLIENT_RETRY_CONTROL_09_003: [If malloc fails, `retry_control_create` shall fail and return NULL]
        LogError("Failed creating the retry control (malloc failed)");
    }
    else
    {
        // Codes_SRS_IOTHUB_CLIENT_RETRY_CONTROL_09_004: [The parameters passed to `retry_control_create` shall be saved into `retry_control`]
        memset(retry_control, 0, sizeof(RETRY_CONTROL_INSTANCE));
        retry_control->policy = policy;
        retry_control->max_retry_time_in_secs = max_retry_time_in_secs;

        // Codes_SRS_IOTHUB_CLIENT_RETRY_CONTROL_09_005: [If `policy` is IOTHUB_CLIENT_RETRY_EXPONENTIAL_BACKOFF or IOTHUB_CLIENT_RETRY_EXPONENTIAL_BACKOFF_WITH_JITTER, `retry_control->initial_wait_time_in_secs` shall be set to 1]
        if (retry_control->policy == IOTHUB_CLIENT_RETRY_EXPONENTIAL_BACKOFF ||
            retry_control->policy == IOTHUB_CLIENT_RETRY_EXPONENTIAL_BACKOFF_WITH_JITTER)
        {
            retry_control->initial_wait_time_in_secs = 1;
        }
        // Codes_SRS_IOTHUB_CLIENT_RETRY_CONTROL_09_006: [Otherwise `retry_control->initial_wait_time_in_secs` shall be set to 5]
        else
        {
            retry_control->initial_wait_time_in_secs = 5;
        }

        // Codes_SRS_IOTHUB_CLIENT_RETRY_CONTROL_09_007: [`retry_control->max_jitter_percent` shall be set to 5]
        retry_control->max_jitter_percent = 5;
        retry_control->max_delay_in_secs = DEFAULT_MAX_DELAY_IN_SECS;

        // Codes_SRS_IOTHUB_CLIENT_RETRY_CONTROL_09_008: [The remaining fields in `retry_control` shall be initialized according to retry_control_reset()]
        retry_control_reset(retry_control);
    }

    // Codes_SRS_IOTHUB_CLIENT_RETRY_CONTROL_09_009: [If no errors occur, `retry_control_create` shall return a handle to `retry_control`]
    return (RETRY_CONTROL_HANDLE)retry_control;
}

void retry_control_destroy(RETRY_CONTROL_HANDLE retry_control_handle)
{
    // Codes_SRS_IOTHUB_CLIENT_RETRY_CONTROL_09_055: [If `retry_control_handle` is NULL, `retry_control_destroy` shall return]
    if (retry_control_handle == NULL)
    {
        LogError("Failed to destroy the retry control (retry_control_handle is NULL)");
    }
    else
    {
        // Codes_SRS_IOTHUB_CLIENT_RETRY_CONTROL_09_056: [`retry_control_destroy` shall destroy `retry_control_handle` using free()]
        free(retry_control_handle);
    }
}

int retry_control_should_retry(RETRY_CONTROL_HANDLE retry_control_handle, RETRY_ACTION* retry_action)
{
    int result;

    // Codes_SRS_IOTHUB_CLIENT_RETRY_CONTROL_09_010: [If `retry_control_handle` or `retry_action` are NULL, `retry_control_should_retry` shall fail and return non-zero]
    if ((retry_control_handle == NULL) || (retry_action == NULL))
    {
        LogError("Failed to evaluate if retry should be attempted (either retry_control_handle (%p) or retry_action (%p) are NULL)", retry_control_handle, retry_action);
        result = MU_FAILURE;
    }
    else
    {
        RETRY_CONTROL_INSTANCE* retry_control = (RETRY_CONTROL_INSTANCE*)retry_control_handle;

        // Codes_SRS_IOTHUB_CLIENT_RETRY_CONTROL_09_027: [If `retry_control->policy` is IOTHUB_CLIENT_RETRY_NONE, retry_action shall be set to RETRY_ACTION_STOP_RETRYING and return immediatelly with result 0]
        if (retry_control->policy == IOTHUB_CLIENT_RETRY_NONE)
        {
            *retry_action = RETRY_ACTION_STOP_RETRYING;
            result = RESULT_OK;
        }
        // Codes_SRS_IOTHUB_CLIENT_RETRY_CONTROL_09_011: [If `retry_control->first_retry_time` is INDEFINITE_TIME, it shall be set using get_time()]
        else if (retry_control->first_retry_time == INDEFINITE_TIME && (retry_control->first_retry_time = get_time(NULL)) == INDEFINITE_TIME)
        {
            // Codes_SRS_IOTHUB_CLIENT_RETRY_CONTROL_09_012: [If get_time() fails, `retry_control_should_retry` shall fail and return non-zero]
            LogError("Failed to evaluate if retry should be attempted (get_time() failed)");
            result = MU_FAILURE;
        }
        // Codes_SRS_IOTHUB_CLIENT_RETRY_CONTROL_09_013: [evaluate_retry_action() shall be invoked]
        else if (evaluate_retry_action(retry_control, retry_action) != RESULT_OK)
        {
            // Codes_SRS_IOTHUB_CLIENT_RETRY_CONTROL_09_014: [If evaluate_retry_action() fails, `retry_control_should_retry` shall fail and return non-zero]
            LogError("Failed to evaluate if retry should be attempted (evaluate_retry_action() failed)");
            result = MU_FAILURE;
        }
        else
        {
            if (*retry_action == RETRY_ACTION_RETRY_NOW)
            {
                // Codes_SRS_IOTHUB_CLIENT_RETRY_CONTROL_09_015: [If `retry_action` is set to RETRY_ACTION_RETRY_NOW, `retry_control->retry_count` shall be incremented by 1]
                retry_control->retry_count++;

                if (retry_control->policy != IOTHUB_CLIENT_RETRY_IMMEDIATE)
                {
                    // Codes_SRS_IOTHUB_CLIENT_RETRY_CONTROL_09_016: [If `retry_action` is set to RETRY_ACTION_RETRY_NOW and policy is not IOTHUB_CLIENT_RETRY_IMMEDIATE, `retry_control->last_retry_time` shall be set using get_time()]
                    retry_control->last_retry_time = get_time(NULL);

                    // Codes_SRS_IOTHUB_CLIENT_RETRY_CONTROL_09_017: [If `retry_action` is set to RETRY_ACTION_RETRY_NOW and policy is not IOTHUB_CLIENT_RETRY_IMMEDIATE, `retry_control->current_wait_time_in_secs` shall be set using calculate_next_wait_time()]
                    retry_control->current_wait_time_in_secs = calculate_next_wait_time(retry_control);
                }
            }

            // Codes_SRS_IOTHUB_CLIENT_RETRY_CONTROL_09_018: [If no errors occur, `retry_control_should_retry` shall return 0]
            result = RESULT_OK;
        }
    }

    return result;
}

int retry_control_set_option(RETRY_CONTROL_HANDLE retry_control_handle, const char* name, const void* value)
{
    int result;

    // Codes_SRS_IOTHUB_CLIENT_RETRY_CONTROL_09_036: [If `retry_control_handle`, `name` or `value` are NULL, `retry_control_set_option` shall fail and return non-zero]
    if (retry_control_handle == NULL || name == NULL || value == NULL)
    {
        LogError("Failed to set option (either retry_state_handle (%p), name (%p) or value (%p) are NULL)", retry_control_handle, name, value);
        result = MU_FAILURE;
    }
    else
    {
        RETRY_CONTROL_INSTANCE* retry_control = (RETRY_CONTROL_INSTANCE*)retry_control_handle;

        if (strcmp(RETRY_CONTROL_OPTION_INITIAL_WAIT_TIME_IN_SECS, name) == 0)
        {
            unsigned int cast_value = *((unsigned int*)value);

            // Codes_SRS_IOTHUB_CLIENT_RETRY_CONTROL_09_037: [If `name` is "initial_wait_time_in_secs" and `value` is less than 1, `retry_control_set_option` shall fail and return non-zero]
            if (cast_value < 1)
            {
                LogError("Failed to set option '%s' (value must be equal or greater to 1)", name);
                result = MU_FAILURE;
            }
            else
            {
                // Codes_SRS_IOTHUB_CLIENT_RETRY_CONTROL_09_038: [If `name` is "initial_wait_time_in_secs", `value` shall be saved on `retry_control->initial_wait_time_in_secs`]
                retry_control->initial_wait_time_in_secs = cast_value;

                // Codes_SRS_IOTHUB_CLIENT_RETRY_CONTROL_09_044: [If no errors occur, retry_control_set_option shall return 0]
                result = RESULT_OK;
            }
        }
        else if (strcmp(RETRY_CONTROL_OPTION_MAX_JITTER_PERCENT, name) == 0)
        {
            unsigned int cast_value = *((unsigned int*)value);

            // Codes_SRS_IOTHUB_CLIENT_RETRY_CONTROL_09_039: [If `name` is "max_jitter_percent" and `value` is less than 0 or greater than 100, `retry_control_set_option` shall fail and return non-zero]
            if (cast_value > 100) // it's unsigned int, it doesn't need to be checked for less than zero.
            {
                LogError("Failed to set option '%s' (value must be in the range 0 to 100)", name);
                result = MU_FAILURE;
            }
            else
            {
                // Codes_SRS_IOTHUB_CLIENT_RETRY_CONTROL_09_040: [If `name` is "max_jitter_percent", value shall be saved on `retry_control->max_jitter_percent`]
                retry_control->max_jitter_percent = cast_value;

                // Codes_SRS_IOTHUB_CLIENT_RETRY_CONTROL_09_044: [If no errors occur, retry_control_set_option shall return 0]
                result = RESULT_OK;
            }
        }
        else if (strcmp(RETRY_CONTROL_OPTION_MAX_DELAY_IN_SECS, name) == 0)
        {
            retry_control->max_delay_in_secs = *((unsigned int*)value);

            result = RESULT_OK;
        }
        else if (strcmp(RETRY_CONTROL_OPTION_SAVED_OPTIONS, name) == 0)
        {
            // Codes_SRS_IOTHUB_CLIENT_RETRY_CONTROL_09_041: [If `name` is "retry_control_options", value shall be fed to `retry_control` using OptionHandler_FeedOptions]
            if (OptionHandler_FeedOptions((OPTIONHANDLER_HANDLE)value, retry_control_handle) != OPTIONHANDLER_OK)
            {
                // Codes_SRS_IOTHUB_CLIENT_RETRY_CONTROL_09_042: [If OptionHandler_FeedOptions fails, `retry_control_set_option` shall fail and return non-zero]
                LogError("messenger_set_option failed (OptionHandler_FeedOptions failed)");
                result = MU_FAILURE;
            }
            else
            {
                // Codes_SRS_IOTHUB_CLIENT_RETRY_CONTROL_09_044: [If no errors occur, `retry_control_set_option` shall return 0]
                result = RESULT_OK;
            }
        }
        // Codes_SRS_IOTHUB_CLIENT_RETRY_CONTROL_09_043: [If `name` is not a supported option, `retry_control_set_option` shall fail and return non-zero]
        else
        {
            LogError("messenger_set_option failed (option with name '%s' is not suppported)", name);
            result = MU_FAILURE;
        }
    }

    return result;
}

OPTIONHANDLER_HANDLE retry_control_retrieve_options(RETRY_CONTROL_HANDLE retry_control_handle)
{
    OPTIONHANDLER_HANDLE result;

    // Codes_SRS_IOTHUB_CLIENT_RETRY_CONTROL_09_045: [If `retry_control_handle`, `retry_control_retrieve_options` shall fail and return NULL]
    if (retry_control_handle == NULL)
    {
        LogError("Failed to retrieve options (retry_state_handle is NULL)");
        result = NULL;
    }
    else
    {
        // Codes_SRS_IOTHUB_CLIENT_RETRY_CONTROL_09_046: [An instance of OPTIONHANDLER_HANDLE (a.k.a. `options`) shall be created using OptionHandler_Create]
        OPTIONHANDLER_HANDLE options = OptionHandler_Create(retry_control_clone_option, retry_control_destroy_option, (pfSetOption)retry_control_set_option);

        if (options == NULL)
        {
            // Codes_SRS_IOTHUB_CLIENT_RETRY_CONTROL_09_047: [If OptionHandler_Create fails, `retry_control_retrieve_options` shall fail and return NULL]
            LogError("Failed to retrieve options (OptionHandler_Create failed)");
            result = NULL;
        }
        else
        {
            RETRY_CONTROL_INSTANCE* retry_control = (RETRY_CONTROL_INSTANCE*)retry_control_handle;

            // Codes_SRS_IOTHUB_CLIENT_RETRY_CONTROL_09_050: [`retry_control->initial_wait_time_in_secs` shall be added to `options` using OptionHandler_Add]
            if (OptionHandler_AddOption(options, RETRY_CONTROL_OPTION_INITIAL_WAIT_TIME_IN_SECS, (void*)&retry_control->initial_wait_time_in_secs) != OPTIONHANDLER_OK)
            {
                // Codes_SRS_IOTHUB_CLIENT_RETRY_CONTROL_09_052: [If any call to OptionHandler_Add fails, `retry_control_retrieve_options` shall fail and return NULL]
                LogError("Failed to retrieve options (OptionHandler_Create failed for option '%s')", RETRY_CONTROL_OPTION_INITIAL_WAIT_TIME_IN_SECS);
                result = NULL;
            }
            // Codes_SRS_IOTHUB_CLIENT_RETRY_CONTROL_09_051: [`retry_control->max_jitter_percent` shall be added to `options` using OptionHandler_Add]
            else if (OptionHandler_AddOption(options, RETRY_CONTROL_OPTION_MAX_JITTER_PERCENT, (void*)&retry_control->max_jitter_percent) != OPTIONHANDLER_OK)
            {
                // Codes_SRS_IOTHUB_CLIENT_RETRY_CONTROL_09_052: [If any call to OptionHandler_Add fails, `retry_control_retrieve_options` shall fail and return NULL]
                LogError("Failed to retrieve options (OptionHandler_Create failed for option '%s')", RETRY_CONTROL_OPTION_INITIAL_WAIT_TIME_IN_SECS);
                result = NULL;
            }
            else
            {
                // Codes_SRS_IOTHUB_CLIENT_RETRY_CONTROL_09_054: [If no errors occur, `retry_control_retrieve_options` shall return the OPTIONHANDLER_HANDLE instance]
                result = options;
            }

            if (result == NULL)
            {
                // Codes_SRS_IOTHUB_CLIENT_RETRY_CONTROL_09_053: [If any failures occur, `retry_control_retrieve_options` shall release any memory it has allocated]
                OptionHandler_Destroy(options);
            }
        }
    }

    return result;
}
