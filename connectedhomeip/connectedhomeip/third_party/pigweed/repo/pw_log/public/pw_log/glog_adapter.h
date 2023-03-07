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

#include "pw_log/internal/glog_adapter.h"
#include "pw_preprocessor/concat.h"

// WARNING: Pigweed strongly recommends sticking to printf-style logging instead
// of C++ stream-style Google Log logging unless absolutely necessary. These
// macros are only provided for compatibility with non-embedded code. See
// https://pigweed.dev/pw_log/ for more details.

// A subset of the streaming Google logging (glog) macros are supported:
// - LOG(glog_level)
// - LOG_IF(glog_level, condition)
//
// The supported glog levels are DEBUG, INFO, WARNING, ERROR, FATAL & DFATAL
//
// This means the following are NOT supported:
// - glog level DFATAL
// - {D,P,SYS}LOG*
// - {,D}VLOG*
// - {,D}CHECK*
// - LOG_EVERY_*, LOG_EVERY_*, LOG_IF_EVERY_*, LOG_FIRST_N
#define LOG(glog_level)                               \
  _PW_LOG_GLOG(_PW_LOG_GLOG_DECLARATION_##glog_level, \
               PW_CONCAT(GlogStreamingLog, __COUNTER__))

#define LOG_IF(glog_level, expr)                         \
  _PW_LOG_GLOG_IF(_PW_LOG_GLOG_DECLARATION_##glog_level, \
                  expr,                                  \
                  PW_CONCAT(GlogStreamingLog, __COUNTER__))
