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

// Configuration macros for the pw_rpc module.
#pragma once

#include <cstddef>
#include <type_traits>

// In client and bidirectional RPCs, pw_rpc clients may signal that they have
// finished sending requests with a CLIENT_STREAM_END packet. While this can be
// useful in some circumstances, it is often not necessary.
//
// This option controls whether or not include a callback that is called when
// the client stream ends. The callback is included in all ServerReader/Writer
// objects as a pw::Function, so may have a significant cost.
#ifndef PW_RPC_CLIENT_STREAM_END_CALLBACK
#define PW_RPC_CLIENT_STREAM_END_CALLBACK 0
#endif  // PW_RPC_CLIENT_STREAM_END_CALLBACK

// The Nanopb-based pw_rpc implementation allocates memory to use for Nanopb
// structs for the request and response protobufs. The template function that
// allocates these structs rounds struct sizes up to this value so that
// different structs can be allocated with the same function. Structs with sizes
// larger than this value cause an extra function to be created, which slightly
// increases code size.
//
// Ideally, this value will be set to the size of the largest Nanopb struct used
// as an RPC request or response. The buffer can be stack or globally allocated
// (see PW_RPC_NANOPB_STRUCT_BUFFER_STACK_ALLOCATE).
#ifndef PW_RPC_NANOPB_STRUCT_MIN_BUFFER_SIZE
#define PW_RPC_NANOPB_STRUCT_MIN_BUFFER_SIZE 64
#endif  // PW_RPC_NANOPB_STRUCT_MIN_BUFFER_SIZE

// Enable global synchronization for RPC calls. If this is set, a backend must
// be configured for pw_sync:mutex.
#ifndef PW_RPC_USE_GLOBAL_MUTEX
#define PW_RPC_USE_GLOBAL_MUTEX 1
#endif  // PW_RPC_USE_GLOBAL_MUTEX

// Whether pw_rpc should use dynamic memory allocation internally. If enabled,
// pw_rpc dynamically allocates channels and its encoding buffers. RPC users may
// use dynamic allocation independently of this option (e.g. to allocate pw_rpc
// call objects).
//
// The semantics for allocating and initializing channels change depending on
// this option. If dynamic allocation is disabled, pw_rpc endpoints (servers or
// clients) use an externally-allocated, fixed-size array of channels.
// That array must include unassigned channels or existing channels must be
// closed to add new channels.
//
// If dynamic allocation is enabled, an span of channels may be passed to the
// endpoint at construction, but these channels are only used to initialize its
// internal std::vector of channels. External channel objects are NOT used by
// the endpoint cannot be updated if dynamic allocation is enabled. No
// unassigned channels should be passed to the endpoint; they will be ignored.
// Any number of channels may be added to the endpoint, without closing existing
// channels, but adding channels will use more memory.
#ifndef PW_RPC_DYNAMIC_ALLOCATION
#define PW_RPC_DYNAMIC_ALLOCATION 0
#endif  // PW_RPC_DYNAMIC_ALLOCATION

// If PW_RPC_DYNAMIC_ALLOCATION is enabled, this macro must expand to a
// container capable of storing objects of the provided type. This container
// will be used internally be pw_rpc. Defaults to std::vector<type>, but may be
// set to any type that supports the following std::vector operations:
//
//   - Construction from iterators
//   - emplace_back()
//   - pop_back()
//   - Range-based for loop iteration
//
#ifndef PW_RPC_DYNAMIC_CONTAINER
#define PW_RPC_DYNAMIC_CONTAINER(type) std::vector<type>
#endif  // PW_RPC_DYNAMIC_CHANNEL_CONTAINER

// If PW_RPC_DYNAMIC_ALLOCATION is enabled, this header file is included in
// files that use PW_RPC_DYNAMIC_CONTAINER. Defaults to <vector>, but may be set
// in conjunction with PW_RPC_DYNAMIC_CONTAINER to use a different container
// type for dynamic allocations in pw_rpc.
#ifndef PW_RPC_DYNAMIC_CONTAINER_INCLUDE
#define PW_RPC_DYNAMIC_CONTAINER_INCLUDE <vector>
#endif  // PW_RPC_DYNAMIC_CHANNEL_CONTAINER

#if PW_RPC_DYNAMIC_ALLOCATION && defined(PW_RPC_ENCODING_BUFFER_SIZE_BYTES)
static_assert(false,
              "PW_RPC_ENCODING_BUFFER_SIZE_BYTES cannot be set if "
              "PW_RPC_DYNAMIC_ALLOCATION is enabled");
#endif  // PW_RPC_DYNAMIC_ALLOCATION && PW_RPC_ENCODING_BUFFER_SIZE_BYTES

// Size of the global RPC packet encoding buffer in bytes.
#ifndef PW_RPC_ENCODING_BUFFER_SIZE_BYTES
#define PW_RPC_ENCODING_BUFFER_SIZE_BYTES 512
#endif  // PW_RPC_ENCODING_BUFFER_SIZE_BYTES

// The log level to use for this module. Logs below this level are omitted.
#ifndef PW_RPC_CONFIG_LOG_LEVEL
#define PW_RPC_CONFIG_LOG_LEVEL PW_LOG_LEVEL_INFO
#endif  // PW_RPC_CONFIG_LOG_LEVEL

// The log module name to use for this module.
#ifndef PW_RPC_CONFIG_LOG_MODULE_NAME
#define PW_RPC_CONFIG_LOG_MODULE_NAME "PW_RPC"
#endif  // PW_RPC_CONFIG_LOG_MODULE_NAME

namespace pw::rpc::cfg {

template <typename...>
constexpr std::bool_constant<PW_RPC_CLIENT_STREAM_END_CALLBACK>
    kClientStreamEndCallbackEnabled;

template <typename...>
constexpr std::bool_constant<PW_RPC_DYNAMIC_ALLOCATION>
    kDynamicAllocationEnabled;

inline constexpr size_t kNanopbStructMinBufferSize =
    PW_RPC_NANOPB_STRUCT_MIN_BUFFER_SIZE;

inline constexpr size_t kEncodingBufferSizeBytes =
    PW_RPC_ENCODING_BUFFER_SIZE_BYTES;

#undef PW_RPC_NANOPB_STRUCT_MIN_BUFFER_SIZE
#undef PW_RPC_ENCODING_BUFFER_SIZE_BYTES

}  // namespace pw::rpc::cfg

// This option determines whether to allocate the Nanopb structs on the stack or
// in a global variable. Globally allocated structs are NOT thread safe, but
// work fine when the RPC server's ProcessPacket function is only called from
// one thread.
#ifndef PW_RPC_NANOPB_STRUCT_BUFFER_STACK_ALLOCATE
#define PW_RPC_NANOPB_STRUCT_BUFFER_STACK_ALLOCATE 1
#endif  // PW_RPC_NANOPB_STRUCT_BUFFER_STACK_ALLOCATE

#if PW_RPC_NANOPB_STRUCT_BUFFER_STACK_ALLOCATE
#define _PW_RPC_NANOPB_STRUCT_STORAGE_CLASS
#else
#define _PW_RPC_NANOPB_STRUCT_STORAGE_CLASS static
#endif  // PW_RPC_NANOPB_STRUCT_BUFFER_STACK_ALLOCATE

#undef PW_RPC_NANOPB_STRUCT_BUFFER_STACK_ALLOCATE
