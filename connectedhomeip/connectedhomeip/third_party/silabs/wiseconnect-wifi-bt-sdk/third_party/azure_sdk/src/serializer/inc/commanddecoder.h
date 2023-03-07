// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef COMMAND_DECODER_H
#define COMMAND_DECODER_H

#include "multitree.h"
#include "schema.h"
#include "agenttypesystem.h"
#include "azure_macro_utils/macro_utils.h"
#include "methodreturn.h"

#ifdef __cplusplus
extern "C" {
#else
#include <stdbool.h>
#endif

#define COMMANDDECODER_RESULT_VALUES \
    COMMANDDECODER_OK, \
    COMMANDDECODER_ERROR, \
    COMMANDDECODER_INVALID_ARG

MU_DEFINE_ENUM_WITHOUT_INVALID(COMMANDDECODER_RESULT, COMMANDDECODER_RESULT_VALUES)

#define EXECUTE_COMMAND_RESULT_VALUES \
EXECUTE_COMMAND_SUCCESS, /*when the final recipient of the command indicates a successful execution*/ \
EXECUTE_COMMAND_FAILED, /*when the final recipient of the command indicates a failure*/ \
EXECUTE_COMMAND_ERROR /*when a transient error either in the final recipient or until the final recipient*/

MU_DEFINE_ENUM_WITHOUT_INVALID(EXECUTE_COMMAND_RESULT, EXECUTE_COMMAND_RESULT_VALUES)




/* Codes_SRS_COMMAND_DECODER_99_001:[ The CommandDecoder module shall expose the following API:] */

typedef struct COMMAND_DECODER_HANDLE_DATA_TAG* COMMAND_DECODER_HANDLE;
typedef EXECUTE_COMMAND_RESULT(*ACTION_CALLBACK_FUNC)(void* actionCallbackContext, const char* relativeActionPath, const char* actionName, size_t argCount, const AGENT_DATA_TYPE* args);
typedef METHODRETURN_HANDLE(*METHOD_CALLBACK_FUNC)(void* methodCallbackContext, const char* relativeMethodPath, const char* methodName, size_t argCount, const AGENT_DATA_TYPE* args);

#include "umock_c/umock_c_prod.h"

MOCKABLE_FUNCTION(,COMMAND_DECODER_HANDLE, CommandDecoder_Create, SCHEMA_MODEL_TYPE_HANDLE, modelHandle, ACTION_CALLBACK_FUNC, actionCallback, void*, actionCallbackContext, METHOD_CALLBACK_FUNC, methodCallback, void*, methodCallbackContext);
MOCKABLE_FUNCTION(,EXECUTE_COMMAND_RESULT, CommandDecoder_ExecuteCommand, COMMAND_DECODER_HANDLE, handle, const char*, command);
MOCKABLE_FUNCTION(,METHODRETURN_HANDLE, CommandDecoder_ExecuteMethod, COMMAND_DECODER_HANDLE, handle, const char*, fullMethodName, const char*, methodPayload);
MOCKABLE_FUNCTION(,void, CommandDecoder_Destroy, COMMAND_DECODER_HANDLE, commandDecoderHandle);

MOCKABLE_FUNCTION(, EXECUTE_COMMAND_RESULT, CommandDecoder_IngestDesiredProperties, void*, startAddress, COMMAND_DECODER_HANDLE, handle, const char*, jsonPayload, bool, parseDesiredNode);

#ifdef __cplusplus
}
#endif

#endif /* COMMAND_DECODER_H */
