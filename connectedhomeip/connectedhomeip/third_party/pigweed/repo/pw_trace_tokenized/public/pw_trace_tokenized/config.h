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

// Configurable options for the tokenized trace module.
#pragma once

// Since not all strings are tokenizeable, labels can be passed as arguments.
// PW_TRACE_CONFIG_ARG_LABEL_SIZE_BYTES configures the maximum number of
// characters to include, if more are provided the string will be clipped.
#ifndef PW_TRACE_CONFIG_ARG_LABEL_SIZE_BYTES
#define PW_TRACE_CONFIG_ARG_LABEL_SIZE_BYTES 20
#endif  // PW_TRACE_CONFIG_ARG_LABEL_SIZE_BYTES

// PW_TRACE_QUEUE_SIZE_EVENTS configures the number of events which can be
// queued up internally. This is needed to support concurrent trace events.
#ifndef PW_TRACE_QUEUE_SIZE_EVENTS
#define PW_TRACE_QUEUE_SIZE_EVENTS 5
#endif  // PW_TRACE_QUEUE_SIZE_EVENTS

// --- Config options for time source ----

// PW_TRACE_TIME_TYPE sets the type for trace time.
#ifndef PW_TRACE_TIME_TYPE
#define PW_TRACE_TIME_TYPE uint32_t
#endif  // PW_TRACE_TIME_TYPE

// PW_TRACE_GET_TIME is the macro which is called to get the current time for a
// trace event. It's default is to use pw_trace_GetTraceTime() which needs to be
// provided by the platform.
#ifndef PW_TRACE_GET_TIME
#define PW_TRACE_GET_TIME() pw_trace_GetTraceTime()
extern PW_TRACE_TIME_TYPE pw_trace_GetTraceTime(void);
#endif  // PW_TRACE_GET_TIME

// PW_TRACE_GET_TIME_TICKS_PER_SECOND is the macro which is called to determine
// the unit of the trace time. It's default is to use
// pw_trace_GetTraceTimeTicksPerSecond() which needs to be provided by the
// platform.
#ifndef PW_TRACE_GET_TIME_TICKS_PER_SECOND
#define PW_TRACE_GET_TIME_TICKS_PER_SECOND() \
  pw_trace_GetTraceTimeTicksPerSecond()
extern size_t pw_trace_GetTraceTimeTicksPerSecond(void);
#endif  // PW_TRACE_GET_TIME_TICKS_PER_SECOND

// PW_TRACE_GET_TIME_DELTA is te macro which is called to determine
// the delta between two PW_TRACE_TIME_TYPE variables. It should return a
// delta of the two times, in the same type.
// The default implementation just subtracts the two, which is suitable if
// values either never wrap, or are unsigned and do not wrap multiple times
// between trace events. If either of these are not the case a different
// implemention should be used.
#ifndef PW_TRACE_GET_TIME_DELTA
#define PW_TRACE_GET_TIME_DELTA(last_time, current_time) \
  ((current_time) - (last_time))
#ifdef __cplusplus
static_assert(
    std::is_unsigned<PW_TRACE_TIME_TYPE>::value,
    "Default time delta implementation only works for unsigned time types.");
#endif  // __cplusplus
#endif  // PW_TRACE_GET_TIME_DELTA

// --- Config options for callbacks ----

// PW_TRACE_CONFIG_MAX_EVENT_CALLBACKS is the maximum number of event callbacks
// which can be registered at a time.
#ifndef PW_TRACE_CONFIG_MAX_EVENT_CALLBACKS
#define PW_TRACE_CONFIG_MAX_EVENT_CALLBACKS 2
#endif  // PW_TRACE_CONFIG_MAX_EVENT_CALLBACKS

// PW_TRACE_CONFIG_MAX_SINKS is the maximum number of encoded event sinks which
// can be registered at a time.
#ifndef PW_TRACE_CONFIG_MAX_SINKS
#define PW_TRACE_CONFIG_MAX_SINKS 2
#endif  // PW_TRACE_CONFIG_MAX_SINKS

// --- Config options for locks ---

// PW_TRACE_LOCK  Is is also called when registering and unregistering callbacks
// and sinks.
#ifndef PW_TRACE_LOCK
#define PW_TRACE_LOCK()
#endif  // PW_TRACE_LOCK

// PW_TRACE_TRY_LOCK is is called when events need to be emptied from the queue,
// if multiple trace events happened at the same time only one task needs to get
// this lock and will empty the queue for all tasks, therefore there is no need
// to block in trace events.
// This should lock the same object as PW_TRACE_LOCK, and be unlocked using
// PW_TRACE_UNLOCK
// Returns true if lock was acquired and false if the lock is currently held and
// could not be aquired.
#ifndef PW_TRACE_TRY_LOCK
#define PW_TRACE_TRY_LOCK() (true)  // Returns true if lock successful
#endif                              // PW_TRACE_TRY_LOCK

#ifndef PW_TRACE_UNLOCK
#define PW_TRACE_UNLOCK()
#endif  // PW_TRACE_UNLOCK

// PW_TRACE_QUEUE_* is used to lock while queueing an event, this is a quick
// copy operation and was designed to be suitable in a critical section to
// avoid unneccessary blocking and task switches.
#ifndef PW_TRACE_QUEUE_LOCK
#define PW_TRACE_QUEUE_LOCK()
#endif  // PW_TRACE_QUEUE_LOCK

#ifndef PW_TRACE_QUEUE_UNLOCK
#define PW_TRACE_QUEUE_UNLOCK()
#endif  // PW_TRACE_QUEUE_UNLOCK

// --- Config options for optional trace buffer ---

// PW_TRACE_BUFFER_SIZE_BYTES is the size in bytes of the optional trace buffer.
// The buffer is automatically registered at boot if the buffer size is not 0.
#ifndef PW_TRACE_BUFFER_SIZE_BYTES
#define PW_TRACE_BUFFER_SIZE_BYTES 256
#endif  // PW_TRACE_BUFFER_SIZE_BYTES

// PW_TRACE_BUFFER_MAX_BLOCK_SIZE_BYTES indicates the maximum size any
// individual encoded trace event could be. This is used internally to buffer up
// a sample before saving into the buffer.
#ifndef PW_TRACE_BUFFER_MAX_BLOCK_SIZE_BYTES
// The below calaculation is provided to help determine a suitable value, using
// the max data size bytes.
#ifndef PW_TRACE_BUFFER_MAX_DATA_SIZE_BYTES
#define PW_TRACE_BUFFER_MAX_DATA_SIZE_BYTES (32)
#endif  // PW_TRACE_BUFFER_MAX_BLOCK_SIZE_BYTES

#ifndef PW_TRACE_BUFFER_MAX_HEADER_SIZE_BYTES
#define PW_TRACE_BUFFER_MAX_HEADER_SIZE_BYTES                                  \
  (pw::varint::kMaxVarint64SizeBytes) +     /* worst case delta time varint */ \
      (sizeof(uint32_t)) +                  /* trace token size */             \
      (pw::varint::kMaxVarint64SizeBytes) + /* worst case trace id varint */
#endif  // PW_TRACE_BUFFER_MAX_HEADER_SIZE_BYTES

#define PW_TRACE_BUFFER_MAX_BLOCK_SIZE_BYTES \
  PW_TRACE_BUFFER_MAX_HEADER_SIZE_BYTES + PW_TRACE_BUFFER_MAX_DATA_SIZE_BYTES
#endif  // PW_TRACE_BUFFER_MAX_BLOCK_SIZE_BYTES
