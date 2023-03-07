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

#include "pw_trace/trace.h"

#include "pw_preprocessor/util.h"
#include "pw_trace_tokenized/trace_callback.h"
#include "pw_trace_tokenized/trace_tokenized.h"
#include "pw_varint/varint.h"

namespace pw {
namespace trace {

TokenizedTraceImpl TokenizedTrace::instance_;
CallbacksImpl Callbacks::instance_;

void TokenizedTraceImpl::HandleTraceEvent(uint32_t trace_token,
                                          EventType event_type,
                                          const char* module,
                                          uint32_t trace_id,
                                          uint8_t flags,
                                          const void* data_buffer,
                                          size_t data_size) {
  // Early exit if disabled and no callbacks are register to receive events
  // while disabled.
  if (!enabled_ && Callbacks::Instance().GetCalledOnEveryEventCount() == 0) {
    return;
  }

  // Create trace event
  PW_TRACE_QUEUE_LOCK();
  if (!event_queue_
           .TryPushBack(trace_token,
                        event_type,
                        module,
                        trace_id,
                        flags,
                        data_buffer,
                        data_size)
           .ok()) {
    // Queue full dropping sample
    // TODO(rgoliver): Allow other strategies, for example: drop oldest, try
    // empty queue, or block.
  }
  PW_TRACE_QUEUE_UNLOCK();

  // Sample is now in queue (if not dropped), try to empty the queue if not
  // already being emptied.
  if (PW_TRACE_TRY_LOCK()) {
    while (!event_queue_.IsEmpty()) {
      HandleNextItemInQueue(event_queue_.PeekFront());
      event_queue_.PopFront();
    }
    PW_TRACE_UNLOCK();
  }
}

void TokenizedTraceImpl::HandleNextItemInQueue(
    const volatile TraceQueue::QueueEventBlock* event_block) {
  // Get next item in queue
  uint32_t trace_token = event_block->trace_token;
  EventType event_type = event_block->event_type;
  const char* module = event_block->module;
  uint32_t trace_id = event_block->trace_id;
  uint8_t flags = event_block->flags;
  const std::byte* data_buffer =
      const_cast<const std::byte*>(event_block->data_buffer);
  size_t data_size = event_block->data_size;

  // Call any event callback which is registered to receive every event.
  pw_trace_TraceEventReturnFlags ret_flags = 0;
  ret_flags |=
      Callbacks::Instance().CallEventCallbacks(CallbacksImpl::kCallOnEveryEvent,
                                               trace_token,
                                               event_type,
                                               module,
                                               trace_id,
                                               flags);
  // Return if disabled.
  if ((PW_TRACE_EVENT_RETURN_FLAGS_SKIP_EVENT & ret_flags) || !enabled_) {
    return;
  }

  // Call any event callback not already called.
  ret_flags |= Callbacks::Instance().CallEventCallbacks(
      CallbacksImpl::kCallOnlyWhenEnabled,
      trace_token,
      event_type,
      module,
      trace_id,
      flags);
  // Return if disabled (from a callback) or if a callback has indicated the
  // sample should be skipped.
  if ((PW_TRACE_EVENT_RETURN_FLAGS_SKIP_EVENT & ret_flags) || !enabled_) {
    return;
  }

  // Create header to store trace info
  static constexpr size_t kMaxHeaderSize =
      sizeof(trace_token) + pw::varint::kMaxVarint64SizeBytes +  // time
      pw::varint::kMaxVarint64SizeBytes;                         // trace_id
  std::byte header[kMaxHeaderSize];
  memcpy(header, &trace_token, sizeof(trace_token));
  size_t header_size = sizeof(trace_token);

  // Compute delta of time elapsed since last trace entry.
  PW_TRACE_TIME_TYPE trace_time = pw_trace_GetTraceTime();
  PW_TRACE_TIME_TYPE delta =
      (last_trace_time_ == 0)
          ? 0
          : PW_TRACE_GET_TIME_DELTA(last_trace_time_, trace_time);
  header_size += pw::varint::Encode(
      delta,
      span<std::byte>(&header[header_size], kMaxHeaderSize - header_size));
  last_trace_time_ = trace_time;

  // Calculate packet id if needed.
  if (PW_TRACE_HAS_TRACE_ID(event_type)) {
    header_size += pw::varint::Encode(
        trace_id,
        span<std::byte>(&header[header_size], kMaxHeaderSize - header_size));
  }

  // Send encoded output to any registered trace sinks.
  Callbacks::Instance().CallSinks(
      span<const std::byte>(header, header_size),
      span<const std::byte>(reinterpret_cast<const std::byte*>(data_buffer),
                            data_size));
  // Disable after processing if an event callback had set the flag.
  if (PW_TRACE_EVENT_RETURN_FLAGS_DISABLE_AFTER_PROCESSING & ret_flags) {
    enabled_ = false;
  }
}

pw_trace_TraceEventReturnFlags CallbacksImpl::CallEventCallbacks(
    CallOnEveryEvent called_on_every_event,
    uint32_t trace_ref,
    EventType event_type,
    const char* module,
    uint32_t trace_id,
    uint8_t flags) {
  pw_trace_TraceEventReturnFlags ret_flags = 0;
  for (size_t i = 0; i < PW_TRACE_CONFIG_MAX_EVENT_CALLBACKS; i++) {
    if (event_callbacks_[i].callback &&
        event_callbacks_[i].called_on_every_event == called_on_every_event) {
      ret_flags |= Callbacks::Instance().GetEventCallback(i)->callback(
          event_callbacks_[i].user_data,
          trace_ref,
          event_type,
          module,
          trace_id,
          flags);
    }
  }
  return ret_flags;
}

void CallbacksImpl::CallSinks(span<const std::byte> header,
                              span<const std::byte> data) {
  for (size_t sink_idx = 0; sink_idx < PW_TRACE_CONFIG_MAX_SINKS; sink_idx++) {
    void* user_data = sink_callbacks_[sink_idx].user_data;
    if (sink_callbacks_[sink_idx].start_block) {
      sink_callbacks_[sink_idx].start_block(user_data,
                                            header.size() + data.size());
    }
    if (sink_callbacks_[sink_idx].add_bytes) {
      sink_callbacks_[sink_idx].add_bytes(
          user_data, header.data(), header.size());
      if (!data.empty()) {
        sink_callbacks_[sink_idx].add_bytes(
            user_data, data.data(), data.size());
      }
    }
    if (sink_callbacks_[sink_idx].end_block) {
      sink_callbacks_[sink_idx].end_block(user_data);
    }
  }
}

pw::Status CallbacksImpl::RegisterSink(SinkStartBlock start_func,
                                       SinkAddBytes add_bytes_func,
                                       SinkEndBlock end_block_func,
                                       void* user_data,
                                       SinkHandle* handle) {
  pw_Status status = PW_STATUS_RESOURCE_EXHAUSTED;
  PW_TRACE_LOCK();
  for (size_t sink_idx = 0; sink_idx < PW_TRACE_CONFIG_MAX_SINKS; sink_idx++) {
    if (IsSinkFree(sink_idx)) {
      sink_callbacks_[sink_idx].start_block = start_func;
      sink_callbacks_[sink_idx].add_bytes = add_bytes_func;
      sink_callbacks_[sink_idx].end_block = end_block_func;
      sink_callbacks_[sink_idx].user_data = user_data;
      if (handle) {
        *handle = sink_idx;
      }
      status = PW_STATUS_OK;
      break;
    }
  }
  PW_TRACE_UNLOCK();
  return status;
}

pw::Status CallbacksImpl::UnregisterSink(SinkHandle handle) {
  PW_TRACE_LOCK();
  if (handle >= PW_TRACE_CONFIG_MAX_SINKS) {
    return PW_STATUS_INVALID_ARGUMENT;
  }
  sink_callbacks_[handle].start_block = nullptr;
  sink_callbacks_[handle].add_bytes = nullptr;
  sink_callbacks_[handle].end_block = nullptr;
  PW_TRACE_UNLOCK();
  return PW_STATUS_OK;
}

pw::Status CallbacksImpl::UnregisterAllSinks() {
  for (size_t sink_idx = 0; sink_idx < PW_TRACE_CONFIG_MAX_SINKS; sink_idx++) {
    UnregisterSink(sink_idx)
        .IgnoreError();  // TODO(b/242598609): Handle Status properly
  }
  return PW_STATUS_OK;
}

CallbacksImpl::SinkCallbacks* CallbacksImpl::GetSink(SinkHandle handle) {
  if (handle >= PW_TRACE_CONFIG_MAX_EVENT_CALLBACKS) {
    return nullptr;
  }
  return &sink_callbacks_[handle];
}

pw::Status CallbacksImpl::RegisterEventCallback(
    EventCallback callback,
    CallOnEveryEvent called_on_every_event,
    void* user_data,
    EventCallbackHandle* handle) {
  pw_Status status = PW_STATUS_RESOURCE_EXHAUSTED;
  PW_TRACE_LOCK();
  for (size_t i = 0; i < PW_TRACE_CONFIG_MAX_EVENT_CALLBACKS; i++) {
    if (event_callbacks_[i].callback == nullptr) {
      event_callbacks_[i].callback = callback;
      event_callbacks_[i].user_data = user_data;
      event_callbacks_[i].called_on_every_event = called_on_every_event;
      called_on_every_event_count_ += called_on_every_event ? 1 : 0;
      if (handle) {
        *handle = i;
      }
      status = PW_STATUS_OK;
      break;
    }
  }
  PW_TRACE_UNLOCK();
  return status;
}

pw::Status CallbacksImpl::UnregisterEventCallback(EventCallbackHandle handle) {
  PW_TRACE_LOCK();
  if (handle >= PW_TRACE_CONFIG_MAX_EVENT_CALLBACKS) {
    return PW_STATUS_INVALID_ARGUMENT;
  }
  event_callbacks_[handle].callback = nullptr;
  event_callbacks_[handle].user_data = nullptr;
  called_on_every_event_count_ +=
      event_callbacks_[handle].called_on_every_event ? 1 : 0;
  event_callbacks_[handle].called_on_every_event = kCallOnlyWhenEnabled;
  PW_TRACE_UNLOCK();
  return PW_STATUS_OK;
}

pw::Status CallbacksImpl::UnregisterAllEventCallbacks() {
  for (size_t i = 0; i < PW_TRACE_CONFIG_MAX_EVENT_CALLBACKS; i++) {
    UnregisterEventCallback(i)
        .IgnoreError();  // TODO(b/242598609): Handle Status properly
  }
  return PW_STATUS_OK;
}

CallbacksImpl::EventCallbacks* CallbacksImpl::GetEventCallback(
    EventCallbackHandle handle) {
  if (handle >= PW_TRACE_CONFIG_MAX_EVENT_CALLBACKS) {
    return nullptr;
  }
  return &event_callbacks_[handle];
}

// C functions

PW_EXTERN_C_START

void pw_trace_Enable(bool enable) { TokenizedTrace::Instance().Enable(enable); }

bool pw_trace_IsEnabled() { return TokenizedTrace::Instance().IsEnabled(); }

void pw_trace_TraceEvent(uint32_t trace_token,
                         pw_trace_EventType event_type,
                         const char* module,
                         uint32_t trace_id,
                         uint8_t flags,
                         const void* data_buffer,
                         size_t data_size) {
  TokenizedTrace::Instance().HandleTraceEvent(
      trace_token, event_type, module, trace_id, flags, data_buffer, data_size);
}

pw_Status pw_trace_RegisterSink(pw_trace_SinkStartBlock start_func,
                                pw_trace_SinkAddBytes add_bytes_func,
                                pw_trace_SinkEndBlock end_block_func,
                                void* user_data,
                                pw_trace_SinkHandle* handle) {
  return Callbacks::Instance()
      .RegisterSink(
          start_func, add_bytes_func, end_block_func, user_data, handle)
      .code();
}

pw_Status pw_trace_UnregisterSink(pw_trace_EventCallbackHandle handle) {
  return Callbacks::Instance().UnregisterSink(handle).code();
}

pw_Status pw_trace_RegisterEventCallback(
    pw_trace_EventCallback callback,
    pw_trace_ShouldCallOnEveryEvent called_on_every_event,
    void* user_data,
    pw_trace_EventCallbackHandle* handle) {
  return Callbacks::Instance()
      .RegisterEventCallback(
          callback,
          static_cast<CallbacksImpl::CallOnEveryEvent>(called_on_every_event),
          user_data,
          handle)
      .code();
}

pw_Status pw_trace_UnregisterEventCallback(
    pw_trace_EventCallbackHandle handle) {
  return Callbacks::Instance().UnregisterEventCallback(handle).code();
}

PW_EXTERN_C_END

}  // namespace trace
}  // namespace pw
