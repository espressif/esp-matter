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

// Controls whether to check if pw::Status values are used. Ununsed Status
// values cause compilation warnings / errors. Calling the nop IgnoreError()
// function silences these warnings.
#ifndef PW_STATUS_CFG_CHECK_IF_USED
#define PW_STATUS_CFG_CHECK_IF_USED 0
#endif  // PW_STATUS_CFG_CHECK_IF_USED

// Set internal macro that optionally adds the [[nodiscard]] attribute.
#if PW_STATUS_CFG_CHECK_IF_USED
#define _PW_STATUS_NO_DISCARD [[nodiscard]]
#else
#define _PW_STATUS_NO_DISCARD
#endif  // PW_STATUS_CFG_CHECK_IF_USED
