// Copyright 2020 The Pigweed Authors
//
// Licensed under the Apache License, Version 2.0 (the "License"); you may not
// use this file except in compliance with the License. You may obtain a copy of
// the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
// WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
// License for the specific language governing permissions and limitations under
// the License.

#include "pw_status/status.h"

#define PW_CASE_RETURN_ENUM_STRING(value) \
  case PW_STATUS_##value:                 \
    return #value

extern "C" const char* pw_StatusString(pw_Status status) {
  switch (status) {
    PW_CASE_RETURN_ENUM_STRING(OK);
    PW_CASE_RETURN_ENUM_STRING(CANCELLED);
    PW_CASE_RETURN_ENUM_STRING(UNKNOWN);
    PW_CASE_RETURN_ENUM_STRING(INVALID_ARGUMENT);
    PW_CASE_RETURN_ENUM_STRING(DEADLINE_EXCEEDED);
    PW_CASE_RETURN_ENUM_STRING(NOT_FOUND);
    PW_CASE_RETURN_ENUM_STRING(ALREADY_EXISTS);
    PW_CASE_RETURN_ENUM_STRING(PERMISSION_DENIED);
    PW_CASE_RETURN_ENUM_STRING(RESOURCE_EXHAUSTED);
    PW_CASE_RETURN_ENUM_STRING(FAILED_PRECONDITION);
    PW_CASE_RETURN_ENUM_STRING(ABORTED);
    PW_CASE_RETURN_ENUM_STRING(OUT_OF_RANGE);
    PW_CASE_RETURN_ENUM_STRING(UNIMPLEMENTED);
    PW_CASE_RETURN_ENUM_STRING(INTERNAL);
    PW_CASE_RETURN_ENUM_STRING(UNAVAILABLE);
    PW_CASE_RETURN_ENUM_STRING(DATA_LOSS);
    PW_CASE_RETURN_ENUM_STRING(UNAUTHENTICATED);
    case PW_STATUS_DO_NOT_USE_RESERVED_FOR_FUTURE_EXPANSION_USE_DEFAULT_IN_SWITCH_INSTEAD_:
    default:
      return "INVALID STATUS";
  }
}

#undef PW_CASE_RETURN_ENUM_STRING
