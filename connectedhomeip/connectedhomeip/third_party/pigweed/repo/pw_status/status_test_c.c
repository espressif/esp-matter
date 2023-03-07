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

#include <string.h>

#include "pw_status/status.h"

pw_Status PassStatusFromC(pw_Status status) { return status; }

pw_Status PassStatusFromCpp(pw_Status status);

#define CHECK_STATUS_FROM_CPP(status) \
  (PW_STATUS_##status != PassStatusFromCpp(PW_STATUS_##status))

int TestStatusFromC(void) {
  int errors = 0;

  errors += CHECK_STATUS_FROM_CPP(OK);
  errors += CHECK_STATUS_FROM_CPP(CANCELLED);
  errors += CHECK_STATUS_FROM_CPP(UNKNOWN);
  errors += CHECK_STATUS_FROM_CPP(INVALID_ARGUMENT);
  errors += CHECK_STATUS_FROM_CPP(DEADLINE_EXCEEDED);
  errors += CHECK_STATUS_FROM_CPP(NOT_FOUND);
  errors += CHECK_STATUS_FROM_CPP(ALREADY_EXISTS);
  errors += CHECK_STATUS_FROM_CPP(PERMISSION_DENIED);
  errors += CHECK_STATUS_FROM_CPP(UNAUTHENTICATED);
  errors += CHECK_STATUS_FROM_CPP(RESOURCE_EXHAUSTED);
  errors += CHECK_STATUS_FROM_CPP(FAILED_PRECONDITION);
  errors += CHECK_STATUS_FROM_CPP(ABORTED);
  errors += CHECK_STATUS_FROM_CPP(OUT_OF_RANGE);
  errors += CHECK_STATUS_FROM_CPP(UNIMPLEMENTED);
  errors += CHECK_STATUS_FROM_CPP(INTERNAL);
  errors += CHECK_STATUS_FROM_CPP(UNAVAILABLE);
  errors += CHECK_STATUS_FROM_CPP(DATA_LOSS);

  return errors;
}

#undef CHECK_STATUS_FROM_CPP

#define CHECK_STATUS_STRING(status) \
  (strcmp(#status, pw_StatusString(PW_STATUS_##status)) != 0)

int TestStatusStringsFromC(void) {
  int errors = 0;

  errors += CHECK_STATUS_STRING(OK);
  errors += CHECK_STATUS_STRING(CANCELLED);
  errors += CHECK_STATUS_STRING(DEADLINE_EXCEEDED);
  errors += CHECK_STATUS_STRING(NOT_FOUND);
  errors += CHECK_STATUS_STRING(ALREADY_EXISTS);
  errors += CHECK_STATUS_STRING(PERMISSION_DENIED);
  errors += CHECK_STATUS_STRING(UNAUTHENTICATED);
  errors += CHECK_STATUS_STRING(RESOURCE_EXHAUSTED);
  errors += CHECK_STATUS_STRING(FAILED_PRECONDITION);
  errors += CHECK_STATUS_STRING(ABORTED);
  errors += CHECK_STATUS_STRING(OUT_OF_RANGE);
  errors += CHECK_STATUS_STRING(UNIMPLEMENTED);
  errors += CHECK_STATUS_STRING(INTERNAL);
  errors += CHECK_STATUS_STRING(UNAVAILABLE);
  errors += CHECK_STATUS_STRING(DATA_LOSS);

  return errors;
}

#undef CHECK_STATUS_STRING
