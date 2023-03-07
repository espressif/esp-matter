// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef CBS_H
#define CBS_H

#include "azure_uamqp_c/session.h"
#include "azure_macro_utils/macro_utils.h"
#include "umock_c/umock_c_prod.h"

#ifdef __cplusplus
extern "C" {
#else
#include <stdbool.h>
#endif /* __cplusplus */

#define CBS_OPERATION_RESULT_VALUES \
    CBS_OPERATION_RESULT_OK, \
    CBS_OPERATION_RESULT_CBS_ERROR, \
    CBS_OPERATION_RESULT_OPERATION_FAILED, \
    CBS_OPERATION_RESULT_INSTANCE_CLOSED

MU_DEFINE_ENUM(CBS_OPERATION_RESULT, CBS_OPERATION_RESULT_VALUES)

#define CBS_OPEN_COMPLETE_RESULT_VALUES \
    CBS_OPEN_OK, \
    CBS_OPEN_ERROR, \
    CBS_OPEN_CANCELLED

MU_DEFINE_ENUM(CBS_OPEN_COMPLETE_RESULT, CBS_OPEN_COMPLETE_RESULT_VALUES)

    typedef struct CBS_INSTANCE_TAG* CBS_HANDLE;
    typedef void(*ON_CBS_OPEN_COMPLETE)(void* context, CBS_OPEN_COMPLETE_RESULT open_complete_result);
    typedef void(*ON_CBS_ERROR)(void* context);
    typedef void(*ON_CBS_OPERATION_COMPLETE)(void* context, CBS_OPERATION_RESULT complete_result, unsigned int status_code, const char* status_description);

    MOCKABLE_FUNCTION(, CBS_HANDLE, cbs_create, SESSION_HANDLE, session);
    MOCKABLE_FUNCTION(, void, cbs_destroy, CBS_HANDLE, cbs);
    MOCKABLE_FUNCTION(, int, cbs_open_async, CBS_HANDLE, cbs, ON_CBS_OPEN_COMPLETE, on_cbs_open_complete, void*, on_cbs_open_complete_context, ON_CBS_ERROR, on_cbs_error, void*, on_cbs_error_context);
    MOCKABLE_FUNCTION(, int, cbs_close, CBS_HANDLE, cbs);
    MOCKABLE_FUNCTION(, int, cbs_put_token_async, CBS_HANDLE, cbs, const char*, type, const char*, audience, const char*, token, ON_CBS_OPERATION_COMPLETE, on_cbs_put_token_complete, void*, on_cbs_put_token_complete_context);
    MOCKABLE_FUNCTION(, int, cbs_delete_token_async, CBS_HANDLE, cbs, const char*, type, const char*, audience, ON_CBS_OPERATION_COMPLETE, on_cbs_delete_token_complete, void*, on_cbs_delete_token_complete_context);
    MOCKABLE_FUNCTION(, int, cbs_set_trace, CBS_HANDLE, cbs, bool, trace_on);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* CBS_H */
