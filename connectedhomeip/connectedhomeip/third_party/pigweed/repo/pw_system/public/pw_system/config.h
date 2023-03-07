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

// PW_SYSTEM_LOG_BUFFER_SIZE is the log buffer size which determines how many
// log entries can be buffered prior to streaming them.
//
// Defaults to 4KiB.
#ifndef PW_SYSTEM_LOG_BUFFER_SIZE
#define PW_SYSTEM_LOG_BUFFER_SIZE 4096
#endif  // PW_SYSTEM_LOG_BUFFER_SIZE

// PW_SYSTEM_MAX_LOG_ENTRY_SIZE limits the proto-encoded log entry size. This
// value might depend on a target interface's MTU.
//
// Defaults to 256B.
#ifndef PW_SYSTEM_MAX_LOG_ENTRY_SIZE
#define PW_SYSTEM_MAX_LOG_ENTRY_SIZE 256
#endif  // PW_SYSTEM_MAX_LOG_ENTRY_SIZE

// PW_SYSTEM_MAX_TRANSMISSION_UNIT target's MTU.
//
// Defaults to 1055 bytes, which is enough to fit 512-byte payloads when using
// HDLC framing.
#ifndef PW_SYSTEM_MAX_TRANSMISSION_UNIT
#define PW_SYSTEM_MAX_TRANSMISSION_UNIT 1055
#endif  // PW_SYSTEM_MAX_TRANSMISSION_UNIT

// PW_SYSTEM_DEFAULT_CHANNEL_ID RPC channel ID to host.
//
// Defaults to 1.
#ifndef PW_SYSTEM_DEFAULT_CHANNEL_ID
#define PW_SYSTEM_DEFAULT_CHANNEL_ID 1
#endif  // PW_SYSTEM_DEFAULT_CHANNEL_ID

// PW_SYSTEM_DEFAULT_RPC_HDLC_ADDRESS RPC HDLC default address.
//
// Defaults to 82.
#ifndef PW_SYSTEM_DEFAULT_RPC_HDLC_ADDRESS
#define PW_SYSTEM_DEFAULT_RPC_HDLC_ADDRESS 82
#endif  // PW_SYSTEM_DEFAULT_RPC_HDLC_ADDRESS

// PW_SYSTEM_ENABLE_THREAD_SNAPSHOT_SERVICE specifies if the thread snapshot
// RPC service is enabled.
//
// Defaults to 1.
#ifndef PW_SYSTEM_ENABLE_THREAD_SNAPSHOT_SERVICE
#define PW_SYSTEM_ENABLE_THREAD_SNAPSHOT_SERVICE 1
#endif  // PW_SYSTEM_ENABLE_THREAD_SNAPSHOT_SERVICE

// PW_SYSTEM_WORK_QUEUE_MAX_ENTRIES specifies the maximum number of work queue
// entries that may be staged at once.
//
// Defaults to 32.
#ifndef PW_SYSTEM_WORK_QUEUE_MAX_ENTRIES
#define PW_SYSTEM_WORK_QUEUE_MAX_ENTRIES 32
#endif  // PW_SYSTEM_WORK_QUEUE_MAX_ENTRIES

// PW_SYSTEM_SOCKET_IO_PORT specifies the port number to use for the socket
// stream implementation of pw_system's I/O interface.
//
// Defaults to 33000.
#ifndef PW_SYSTEM_SOCKET_IO_PORT
#define PW_SYSTEM_SOCKET_IO_PORT 33000
#endif  // PW_SYSTEM_SOCKET_IO_PORT
