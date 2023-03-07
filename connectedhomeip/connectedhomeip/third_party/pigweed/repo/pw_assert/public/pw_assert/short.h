// Copyright 2021 The Pigweed Authors
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
#pragma once

#include "pw_assert/check.h"

// Optional short CHECK name definitions

// clang-format off
// Checks that always run even in production.
#define CRASH                 PW_CRASH
#define CHECK                 PW_CHECK
#define CHECK_PTR_LE          PW_CHECK_PTR_LE
#define CHECK_PTR_LT          PW_CHECK_PTR_LT
#define CHECK_PTR_GE          PW_CHECK_PTR_GE
#define CHECK_PTR_GT          PW_CHECK_PTR_GT
#define CHECK_PTR_EQ          PW_CHECK_PTR_EQ
#define CHECK_PTR_NE          PW_CHECK_PTR_NE
#define CHECK_NOTNULL         PW_CHECK_NOTNULL
#define CHECK_INT_LE          PW_CHECK_INT_LE
#define CHECK_INT_LT          PW_CHECK_INT_LT
#define CHECK_INT_GE          PW_CHECK_INT_GE
#define CHECK_INT_GT          PW_CHECK_INT_GT
#define CHECK_INT_EQ          PW_CHECK_INT_EQ
#define CHECK_INT_NE          PW_CHECK_INT_NE
#define CHECK_UINT_LE         PW_CHECK_UINT_LE
#define CHECK_UINT_LT         PW_CHECK_UINT_LT
#define CHECK_UINT_GE         PW_CHECK_UINT_GE
#define CHECK_UINT_GT         PW_CHECK_UINT_GT
#define CHECK_UINT_EQ         PW_CHECK_UINT_EQ
#define CHECK_UINT_NE         PW_CHECK_UINT_NE
#define CHECK_FLOAT_NEAR      PW_CHECK_FLOAT_NEAR
#define CHECK_FLOAT_EXACT_LE  PW_CHECK_FLOAT_EXACT_LE
#define CHECK_FLOAT_EXACT_LT  PW_CHECK_FLOAT_EXACT_LT
#define CHECK_FLOAT_EXACT_GE  PW_CHECK_FLOAT_EXACT_GE
#define CHECK_FLOAT_EXACT_GT  PW_CHECK_FLOAT_EXACT_GT
#define CHECK_FLOAT_EXACT_EQ  PW_CHECK_FLOAT_EXACT_EQ
#define CHECK_FLOAT_EXACT_NE  PW_CHECK_FLOAT_EXACT_NE
#define CHECK_OK              PW_CHECK_OK

// Checks that are disabled if NDEBUG is not defined.
#define DCHECK                PW_DCHECK
#define DCHECK_PTR_LE         PW_DCHECK_PTR_LE
#define DCHECK_PTR_LT         PW_DCHECK_PTR_LT
#define DCHECK_PTR_GE         PW_DCHECK_PTR_GE
#define DCHECK_PTR_GT         PW_DCHECK_PTR_GT
#define DCHECK_PTR_EQ         PW_DCHECK_PTR_EQ
#define DCHECK_PTR_NE         PW_DCHECK_PTR_NE
#define DCHECK_NOTNULL        PW_DCHECK_NOTNULL
#define DCHECK_INT_LE         PW_DCHECK_INT_LE
#define DCHECK_INT_LT         PW_DCHECK_INT_LT
#define DCHECK_INT_GE         PW_DCHECK_INT_GE
#define DCHECK_INT_GT         PW_DCHECK_INT_GT
#define DCHECK_INT_EQ         PW_DCHECK_INT_EQ
#define DCHECK_INT_NE         PW_DCHECK_INT_NE
#define DCHECK_UINT_LE        PW_DCHECK_UINT_LE
#define DCHECK_UINT_LT        PW_DCHECK_UINT_LT
#define DCHECK_UINT_GE        PW_DCHECK_UINT_GE
#define DCHECK_UINT_GT        PW_DCHECK_UINT_GT
#define DCHECK_UINT_EQ        PW_DCHECK_UINT_EQ
#define DCHECK_UINT_NE        PW_DCHECK_UINT_NE
#define DCHECK_FLOAT_NEAR     PW_DCHECK_FLOAT_NEAR
#define DCHECK_FLOAT_EXACT_LT PW_DCHECK_FLOAT_EXACT_LT
#define DCHECK_FLOAT_EXACT_LE PW_DCHECK_FLOAT_EXACT_LE
#define DCHECK_FLOAT_EXACT_GT PW_DCHECK_FLOAT_EXACT_GT
#define DCHECK_FLOAT_EXACT_GE PW_DCHECK_FLOAT_EXACT_GE
#define DCHECK_FLOAT_EXACT_EQ PW_DCHECK_FLOAT_EXACT_EQ
#define DCHECK_FLOAT_EXACT_NE PW_DCHECK_FLOAT_EXACT_NE
#define DCHECK_OK             PW_DCHECK_OK
// clang-format on
