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

// Configuration macros for the pw_rpc module.
#pragma once

#include <cstddef>

// Log filter modules are optionally tokenized, and thus their backing on-device
// container can have different sizes. A token may be represented by a 32-bit
// integer (though it is usually 2 bytes). Default the max module name size to
// 4 bytes.
#ifndef PW_LOG_RPC_CONFIG_MAX_FILTER_RULE_MODULE_NAME_SIZE
#define PW_LOG_RPC_CONFIG_MAX_FILTER_RULE_MODULE_NAME_SIZE 4
#endif  // PW_LOG_RPC_CONFIG_MAX_FILTER_RULE_MODULE_NAME_SIZE

// Log filter threads are optionally tokenized,thus their backing on-device
// container can have different sizes. A token may be represented by a 32-bit
// integer, usually two. Default the max thread size to
// 10 bytes.
#ifndef PW_LOG_RPC_CONFIG_MAX_FILTER_RULE_THREAD_NAME_SIZE
#define PW_LOG_RPC_CONFIG_MAX_FILTER_RULE_THREAD_NAME_SIZE 10
#endif  // PW_LOG_RPC_CONFIG_MAX_FILTER_RULE_THREAD_NAME_SIZE

// Log filter IDs are optionally tokenized, and thus their backing on-device
// container can have different sizes. A token may be represented by a 32-bit
// integer (though it is usually 2 bytes). Default the max module name size to
// 4 bytes.
#ifndef PW_LOG_RPC_CONFIG_MAX_FILTER_ID_SIZE
#define PW_LOG_RPC_CONFIG_MAX_FILTER_ID_SIZE 4
#endif  // PW_LOG_RPC_CONFIG_MAX_FILTER_ID_SIZE

// The log level to use for this module. Logs below this level are omitted.
#ifndef PW_LOG_RPC_CONFIG_LOG_LEVEL
#define PW_LOG_RPC_CONFIG_LOG_LEVEL PW_LOG_LEVEL_INFO
#endif  // PW_LOG_RPC_CONFIG_LOG_LEVEL

// The log module name to use for this module.
#ifndef PW_LOG_RPC_CONFIG_LOG_MODULE_NAME
#define PW_LOG_RPC_CONFIG_LOG_MODULE_NAME "PW_LOG_RPC"
#endif  // PW_LOG_RPC_CONFIG_LOG_MODULE_NAME

// Messages to descrive the log drop reasons.
// See https://pigweed.dev/pw_log_rpc/#log-drops
//
// Message for when an entry could not be added to the MultiSink.
#ifndef PW_LOG_RPC_INGRESS_ERROR_MSG
#define PW_LOG_RPC_INGRESS_ERROR_MSG "Ingress error"
#endif  // PW_LOG_RPC_INGRESS_ERROR_MSG

// Message for when a drain drains too slow and has to be advanced, dropping
// logs.
#ifndef PW_LOG_RPC_SLOW_DRAIN_MSG
#define PW_LOG_RPC_SLOW_DRAIN_MSG "Slow drain"
#endif  // PW_LOG_RPC_SLOW_DRAIN_MSG

// Message for when a is too too large to fit in the outbound buffer, so it is
// dropped.
#ifndef PW_LOG_RPC_SMALL_OUTBOUND_BUFFER_MSG
#define PW_LOG_RPC_SMALL_OUTBOUND_BUFFER_MSG "Outbound log buffer too small"
#endif  // PW_LOG_RPC_SMALL_OUTBOUND_BUFFER_MSG

// Message for when the log entry in the MultiSink is too large to be peeked or
// popped out, so it is dropped.
#ifndef PW_LOG_RPC_SMALL_STACK_BUFFER_MSG
#define PW_LOG_RPC_SMALL_STACK_BUFFER_MSG "Stack log buffer too small"
#endif  // PW_LOG_RPC_SMALL_STACK_BUFFER_MSG

// Message for when a bulk of logs cannot be sent due to a writer error.
#ifndef PW_LOG_RPC_WRITER_ERROR_MSG
#define PW_LOG_RPC_WRITER_ERROR_MSG "Writer error"
#endif  // PW_LOG_RPC_WRITER_ERROR_MSG

namespace pw::log_rpc::cfg {
inline constexpr size_t kMaxModuleNameBytes =
    PW_LOG_RPC_CONFIG_MAX_FILTER_RULE_MODULE_NAME_SIZE;

inline constexpr size_t kMaxFilterIdBytes =
    PW_LOG_RPC_CONFIG_MAX_FILTER_ID_SIZE;

inline constexpr size_t kMaxThreadNameBytes =
    PW_LOG_RPC_CONFIG_MAX_FILTER_RULE_THREAD_NAME_SIZE;
}  // namespace pw::log_rpc::cfg