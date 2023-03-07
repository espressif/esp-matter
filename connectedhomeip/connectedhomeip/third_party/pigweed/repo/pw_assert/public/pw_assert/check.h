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
// =============================================================================
//
// This file describes Pigweed's public user-facing assert API.
//
// THIS API IS NOT STABLE OR COMPLETE! NEITHER FACADE NOR BACKEND API!
//
#pragma once

#include "pw_preprocessor/arguments.h"

// The pw_assert public API:
//
//   Trigger a crash with a message. Replaces LOG_FATAL() in other systems.
//   PW_CRASH(msg, ...)
//
//   In all below cases, the message argument is optional:
//   PW_CHECK_INT_LE(x, y) or
//   PW_CHECK_INT_LE(x, y, "Was booting %s subsystem", subsystem_name)
//
//   Asserts the condition, crashes on failure. Equivalent to assert.
//   PW_CHECK(condition) or
//   PW_CHECK(condition, msg, ...)
//
//   Some common typed checks.
//   PW_CHECK_OK(status, msg, ...)  Asserts status == PW_STATUS_OK
//   PW_CHECK_NOTNULL(ptr, msg, ...)  Asserts ptr != NULL
//
//   In many cases an assert is a binary comparison. In those cases, using the
//   special binary assert macros below for <, <=, >, >=, == enables reporting
//   the values of the operands in addition to the string of the condition.
//
//   Binary comparison asserts for 'int' type ("%d" in format strings):
//   PW_CHECK_INT_LE(a, b, msg, ...)  Asserts a <= b
//   PW_CHECK_INT_LT(a, b, msg, ...)  Asserts a <  b
//   PW_CHECK_INT_GE(a, b, msg, ...)  Asserts a >= b
//   PW_CHECK_INT_GT(a, b, msg, ...)  Asserts a >  b
//   PW_CHECK_INT_EQ(a, b, msg, ...)  Asserts a == b
//   PW_CHECK_INT_NE(a, b, msg, ...)  Asserts a != b
//
//   Binary comparison asserts for 'unsigned int' type ("%u" in format strings):
//   PW_CHECK_UINT_LE(a, b, msg, ...)  Asserts a <= b
//   PW_CHECK_UINT_LT(a, b, msg, ...)  Asserts a <  b
//   PW_CHECK_UINT_GE(a, b, msg, ...)  Asserts a >= b
//   PW_CHECK_UINT_GT(a, b, msg, ...)  Asserts a >  b
//   PW_CHECK_UINT_EQ(a, b, msg, ...)  Asserts a == b
//   PW_CHECK_UINT_NE(a, b, msg, ...)  Asserts a != b
//
//   Binary comparison asserts for 'void*' type ("%p" in format strings):
//   PW_CHECK_PTR_LE(a, b, msg, ...)  Asserts a <= b
//   PW_CHECK_PTR_LT(a, b, msg, ...)  Asserts a <  b
//   PW_CHECK_PTR_GE(a, b, msg, ...)  Asserts a >= b
//   PW_CHECK_PTR_GT(a, b, msg, ...)  Asserts a >  b
//   PW_CHECK_PTR_EQ(a, b, msg, ...)  Asserts a == b
//   PW_CHECK_PTR_NE(a, b, msg, ...)  Asserts a != b
//
//   Binary comparison asserts for 'float' type ("%f" in format strings):
//   PW_CHECK_FLOAT_NEAR(a, b, abs_tolerance, msg, ...)
//     Asserts (a >= (b - abs_tolerance)) && (a <= (b + abs_tolerance))
//   PW_CHECK_FLOAT_EXACT_LE(a, b, msg, ...)  Asserts a <= b
//   PW_CHECK_FLOAT_EXACT_LT(a, b, msg, ...)  Asserts a <  b
//   PW_CHECK_FLOAT_EXACT_GE(a, b, msg, ...)  Asserts a >= b
//   PW_CHECK_FLOAT_EXACT_GT(a, b, msg, ...)  Asserts a >  b
//   PW_CHECK_FLOAT_EXACT_EQ(a, b, msg, ...)  Asserts a == b
//   PW_CHECK_FLOAT_EXACT_NE(a, b, msg, ...)  Asserts a != b
//
//   The above CHECK_*_*() are also available in DCHECK variants, which will
//   only evaluate their arguments and trigger if the NDEBUG macro is defined.
//
//   Note: For float, proper comparator checks which take floating point
//   precision and ergo error accumulation into account are not provided on
//   purpose as this comes with some complexity and requires application
//   specific tolerances in terms of Units of Least Precision (ULP). Instead,
//   we recommend developers carefully consider how floating point precision and
//   error impact the data they are bounding and whether CHECKs are appropriate.
//
//   Note: PW_CRASH is the equivalent of LOG_FATAL in other systems, where a
//   device crash is triggered with a message. In Pigweed, logging and
//   crashing/asserting are separated. There is a LOG_CRITICAL level in the
//   logging module, but it does not have side effects; for LOG_FATAL, instead
//   use this macro (PW_CRASH).
//
// The public macro definitions are split out into an impl file to facilitate
// testing the facade logic directly, without going through the facade/backend
// build facilities.
#include "pw_assert/internal/check_impl.h"

// The pw_assert_backend must provide these macros:
//
//   PW_HANDLE_CRASH(msg, ...)
//   PW_HANDLE_ASSERT_FAILURE(condition, msg, ...)
//   PW_HANDLE_ASSERT_BINARY_COMPARE_FAILURE(a, op, b, type_fmt, msg, ...)
//
//   The low level functionality of triggering a crash, rebooting a device,
//   collecting information, or hanging out in a while(1) loop, must be
//   provided by the underlying assert backend as part of the crash or assert
//   failure handling.
//
//   Note that for the assert failures, the handler should assume the assert
//   has already failed (the facade checks the condition before delegating).
//
#include "pw_assert_backend/check_backend.h"
