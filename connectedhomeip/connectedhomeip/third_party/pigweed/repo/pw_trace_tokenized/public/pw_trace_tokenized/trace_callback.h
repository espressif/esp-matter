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
//==============================================================================
//
// The file provides the API for working with callbacks and sinks for the
// tokenized trace module.

#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "pw_span/span.h"
#include "pw_status/status.h"
#include "pw_trace_tokenized/config.h"
#include "pw_trace_tokenized/trace_tokenized.h"

PW_EXTERN_C_START
// The pw_trace_EventCallback is called before the sample is encoded or sent
// to the sinks. Bits in the return argument can be set to change the behaviour
// of tracing.
//    - PW_TRACE_EVENT_RETURN_FLAGS_SKIP_EVENT can optionally be set true to
//    skip this sample.
//    - PW_TRACE_EVENT_RETURN_FLAGS_DISABLE_AFTER_PROCESSING can be set true to
//      disable tracing after this sample.
//
// When registering the callback the parameter 'called_on_every_event' is used
// to indicate if the callback should be called even when tracing is disabled.
// This behaviour is useful to implment a tracing behaviour, where tracing can
// turn on when a specific event occurs.
//
// If a handle pointer is provided it will be set to a value, which can be later
// used to unregister the callback.
//
// The user_data pointer is provider for use by the application, it can be used
// to allow a single function callback to be registered multiple times but act
// differently by providing it with different context objects as pointers.
//
// NOTE: Since callbacks are called within the trace event lock, they should not
// register/unregister sinks or callbacks or trigger other trace events.
typedef enum {
  PW_TRACE_CALL_ONLY_WHEN_ENABLED = 0,
  PW_TRACE_CALL_ON_EVERY_EVENT = 1,
} pw_trace_ShouldCallOnEveryEvent;

enum {
  PW_TRACE_EVENT_RETURN_FLAGS_NONE = 0,
  PW_TRACE_EVENT_RETURN_FLAGS_SKIP_EVENT = 1 << 0,
  PW_TRACE_EVENT_RETURN_FLAGS_DISABLE_AFTER_PROCESSING = 1 << 1
};
typedef uint32_t pw_trace_TraceEventReturnFlags;

typedef size_t pw_trace_EventCallbackHandle;
typedef pw_trace_TraceEventReturnFlags (*pw_trace_EventCallback)(
    void* user_data,
    uint32_t trace_ref,
    pw_trace_EventType event_type,
    const char* module,
    uint32_t trace_id,
    uint8_t flags);

pw_Status pw_trace_RegisterEventCallback(
    pw_trace_EventCallback callback,
    pw_trace_ShouldCallOnEveryEvent called_on_every_event,
    void* user_data,
    pw_trace_EventCallbackHandle* handle);

// pw_trace_UnregisterEventCallback will cause the callback to not receive any
// more events.
pw_Status pw_trace_UnregisterEventCallback(pw_trace_EventCallbackHandle handle);

// pw_trace_Sink* is called after the trace event is encoded.
// Trace will internally handle locking, so every Start event will have a
// matching End event before another sequence is started.
// The number of bytes sent to AddBytes will be the number provided at the
// start, allowing buffers to allocate the required amount at the start when
// necessary.
//
// If OkStatus() is not returned from Start, the events bytes will be skipped.
//
// NOTE: Called while tracing is locked (which might be a critical section
// depending on application), so quick/simple operations only. One trace event
// might result in multiple callbacks if the data is split up.
//
// If a handle pointer is provided it will be set to a value, which can be later
// used to unregister the callback.
//
// The user_data pointer is provider for use by the application, it can be used
// to allow a single function callback to be registered multiple times but act
// differently by providing it with different user_data values.
//
// NOTE: Since callbacks are called within the trace event lock, they should not
// register/unregister sinks or callbacks or trigger other trace events.
typedef void (*pw_trace_SinkStartBlock)(void* user_data, size_t size);
typedef void (*pw_trace_SinkAddBytes)(void* user_data,
                                      const void* bytes,
                                      size_t size);
typedef void (*pw_trace_SinkEndBlock)(void* user_data);
typedef size_t pw_trace_SinkHandle;
pw_Status pw_trace_RegisterSink(pw_trace_SinkStartBlock start_func,
                                pw_trace_SinkAddBytes add_bytes_func,
                                pw_trace_SinkEndBlock end_block_func,
                                void* user_data,
                                pw_trace_SinkHandle* handle);

// pw_trace_UnregisterSink will cause the sink to stop receiving trace data.
pw_Status pw_trace_UnregisterSink(pw_trace_SinkHandle handle);

PW_EXTERN_C_END

#ifdef __cplusplus
namespace pw {
namespace trace {

class CallbacksImpl {
 public:
  enum CallOnEveryEvent {
    kCallOnlyWhenEnabled = PW_TRACE_CALL_ONLY_WHEN_ENABLED,
    kCallOnEveryEvent = PW_TRACE_CALL_ON_EVERY_EVENT,
  };
  using SinkStartBlock = pw_trace_SinkStartBlock;
  using SinkAddBytes = pw_trace_SinkAddBytes;
  using SinkEndBlock = pw_trace_SinkEndBlock;
  using SinkHandle = pw_trace_SinkHandle;
  struct SinkCallbacks {
    void* user_data;
    SinkStartBlock start_block;
    SinkAddBytes add_bytes;
    SinkEndBlock end_block;
  };
  using EventCallback = pw_trace_EventCallback;
  using EventCallbackHandle = pw_trace_EventCallbackHandle;
  struct EventCallbacks {
    void* user_data;
    EventCallback callback;
    CallOnEveryEvent called_on_every_event;
  };

  pw::Status RegisterSink(SinkStartBlock start_func,
                          SinkAddBytes add_bytes_func,
                          SinkEndBlock end_block_func,
                          void* user_data = nullptr,
                          SinkHandle* handle = nullptr);
  pw::Status UnregisterSink(SinkHandle handle);
  pw::Status UnregisterAllSinks();
  SinkCallbacks* GetSink(SinkHandle handle);
  void CallSinks(span<const std::byte> header, span<const std::byte> data);

  pw::Status RegisterEventCallback(
      EventCallback callback,
      CallOnEveryEvent called_on_every_event = kCallOnlyWhenEnabled,
      void* user_data = nullptr,
      EventCallbackHandle* handle = nullptr);
  pw::Status UnregisterEventCallback(EventCallbackHandle handle);
  pw::Status UnregisterAllEventCallbacks();
  EventCallbacks* GetEventCallback(EventCallbackHandle handle);
  pw_trace_TraceEventReturnFlags CallEventCallbacks(
      CallOnEveryEvent called_on_every_event,
      uint32_t trace_ref,
      EventType event_type,
      const char* module,
      uint32_t trace_id,
      uint8_t flags);
  size_t GetCalledOnEveryEventCount() const {
    return called_on_every_event_count_;
  }

 private:
  EventCallbacks event_callbacks_[PW_TRACE_CONFIG_MAX_EVENT_CALLBACKS];
  SinkCallbacks sink_callbacks_[PW_TRACE_CONFIG_MAX_SINKS];
  size_t called_on_every_event_count_ = 0;

  bool IsSinkFree(pw_trace_SinkHandle handle) {
    return sink_callbacks_[handle].start_block == nullptr &&
           sink_callbacks_[handle].add_bytes == nullptr &&
           sink_callbacks_[handle].end_block == nullptr;
  }
};

// A singleton object of the CallbacksImpl class which can be used to
// interface with trace using the C++ API.
// Example: pw::trace::Callbacks::Instance().UnregisterAllSinks();
class Callbacks {
 public:
  static CallbacksImpl& Instance() { return instance_; }

 private:
  static CallbacksImpl instance_;
};

// This is a convenience class to register the callback when the object is
// created. For example if the callback should always be registered this can be
// created as a global object to avoid needing to call register directly.
class RegisterCallbackWhenCreated {
 public:
  RegisterCallbackWhenCreated(
      CallbacksImpl::EventCallback event_callback,
      CallbacksImpl::CallOnEveryEvent called_on_every_event =
          CallbacksImpl::kCallOnlyWhenEnabled,
      void* user_data = nullptr) {
    Callbacks::Instance()
        .RegisterEventCallback(event_callback, called_on_every_event, user_data)
        .IgnoreError();  // TODO(b/242598609): Handle Status properly
  }
  RegisterCallbackWhenCreated(CallbacksImpl::SinkStartBlock sink_start,
                              CallbacksImpl::SinkAddBytes sink_add_bytes,
                              CallbacksImpl::SinkEndBlock sink_end,
                              void* user_data = nullptr) {
    Callbacks::Instance()
        .RegisterSink(sink_start, sink_add_bytes, sink_end, user_data)
        .IgnoreError();  // TODO(b/242598609): Handle Status properly
  }
};

}  // namespace trace
}  // namespace pw
#endif  // __cplusplus
