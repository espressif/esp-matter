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

// PW_ASSERT_ENABLE_DEBUG controls whether DCHECKs and DASSERTs are enabled.
//
// This block defines PW_ASSERT_ENABLE_DEBUG if it is not already, taking into
// account traditional NDEBUG macro.
#if !defined(PW_ASSERT_ENABLE_DEBUG)
#if defined(NDEBUG)
// Release mode; remove all DCHECK*() and DASSERT() asserts.
#define PW_ASSERT_ENABLE_DEBUG 0
#else
// Debug mode; keep all DCHECK*() and DASSERT() asserts.
#define PW_ASSERT_ENABLE_DEBUG 1
#endif  // defined (NDEBUG)
#endif  // !defined(PW_ASSERT_ENABLE_DEBUG)

// PW_ASSERT_CAPTURE_VALUES controls whether the evaluated values of a CHECK are
// captured in the final string. Disabling this will reduce codesize at CHECK
// callsites.
#if !defined(PW_ASSERT_CAPTURE_VALUES)
#define PW_ASSERT_CAPTURE_VALUES 1
#endif  // !defined(PW_ASSERT_CAPTURE_VALUES)
