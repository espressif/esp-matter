// Copyright 2022 The Pigweed Authors
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

// The size of the stack-allocated buffer used by the Google Logging (glog)
// macros. This only affects the glog macros provided through pw_log/glog.h.
//
// Pigweed strongly recommends sticking to printf-style logging instead
// of C++ stream-style Google Log logging unless absolutely necessary. The glog
// macros are only provided for compatibility with non-embedded code. See
// https://pigweed.dev/pw_log/ for more details.
//
// Undersizing this buffer will result in truncated log messages.
#ifndef PW_LOG_CFG_GLOG_BUFFER_SIZE_BYTES
#define PW_LOG_CFG_GLOG_BUFFER_SIZE_BYTES 128
#endif  // PW_LOG_CFG_GLOG_BUFFER_SIZE_BYTES
