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

// clang-format off
#define PW_TRACE_MODULE_NAME "TST"

#include "pw_trace/trace.h"
#include "pw_trace_tokenized/trace_tokenized.h"
#include "pw_trace_tokenized/trace_callback.h"
// clang-format on

#include <deque>

#include "gtest/gtest.h"

namespace {

// These are line numbers for the functions below. Moving these functions to
// other lines will require updating these macros.
#define TRACE_FUNCTION_LINE 35
#define TRACE_FUNCTION_GROUP_LINE 36
#define TRACE_FUNCTION_ID_LINE 38

void TraceFunction() { PW_TRACE_FUNCTION(); }
void TraceFunctionGroup() { PW_TRACE_FUNCTION("FunctionGroup"); }
void TraceFunctionTraceId(uint32_t id) {
  PW_TRACE_FUNCTION("FunctionGroup", id);
}

// This trace test interface registers as a trace callback to capture trace
// events to verify extpected behaviour. It also supports testing common actions
// within the callback.
class TraceTestInterface {
 public:
  struct TraceInfo {
    uint32_t trace_ref;
    pw::trace::EventType event_type;
    const char* module;
    uint32_t trace_id;
    bool operator==(const TraceInfo& b) const {
      return trace_ref == b.trace_ref && event_type == b.event_type &&
             module == b.module && trace_id == b.trace_id;
    }
  };

  TraceTestInterface() {
    PW_TRACE_SET_ENABLED(true);
    EXPECT_EQ(pw::OkStatus(),
              pw::trace::Callbacks::Instance().RegisterSink(TraceSinkStartBlock,
                                                            TraceSinkAddBytes,
                                                            TraceSinkEndBlock,
                                                            this,
                                                            &sink_handle_));
    EXPECT_EQ(pw::OkStatus(),
              pw::trace::Callbacks::Instance().RegisterEventCallback(
                  TraceEventCallback,
                  pw::trace::CallbacksImpl::kCallOnlyWhenEnabled,
                  this,
                  &event_callback_handle_));
  }
  ~TraceTestInterface() {
    EXPECT_EQ(pw::OkStatus(),
              pw::trace::Callbacks::Instance().UnregisterSink(sink_handle_));
    EXPECT_EQ(pw::OkStatus(),
              pw::trace::Callbacks::Instance().UnregisterEventCallback(
                  event_callback_handle_));
  }
  // ActionOnEvent will perform a specific action within the callback when an
  // event matches one of the characteristics of event_match_.
  enum class ActionOnEvent { None, Enable, Disable, DisableAfter, Skip };
  void SetCallbackEventAction(ActionOnEvent action, TraceInfo event) {
    action_ = action;
    event_match_ = event;
  }

  // The trace event callback will save the trace event info and add it to
  // buffer_ in the TraceSink callback, that way it only gets added to the
  // buffer if tracing is enabled and the sample was not surpressed.
  static pw_trace_TraceEventReturnFlags TraceEventCallback(
      void* user_data,
      uint32_t trace_ref,
      pw_trace_EventType event_type,
      const char* module,
      uint32_t trace_id,
      uint8_t /* flags */) {
    TraceTestInterface* test_interface =
        reinterpret_cast<TraceTestInterface*>(user_data);
    pw_trace_TraceEventReturnFlags ret = 0;
    if (test_interface->action_ != ActionOnEvent::None &&
        (test_interface->event_match_.trace_ref == trace_ref ||
         test_interface->event_match_.event_type == event_type ||
         test_interface->event_match_.module == module ||
         (trace_id != PW_TRACE_TRACE_ID_DEFAULT &&
          test_interface->event_match_.trace_id == trace_id))) {
      if (test_interface->action_ == ActionOnEvent::Skip) {
        ret |= PW_TRACE_EVENT_RETURN_FLAGS_SKIP_EVENT;
      } else if (test_interface->action_ == ActionOnEvent::Enable) {
        PW_TRACE_SET_ENABLED(true);
      } else if (test_interface->action_ == ActionOnEvent::Disable) {
        PW_TRACE_SET_ENABLED(false);
      } else if (test_interface->action_ == ActionOnEvent::DisableAfter) {
        ret |= PW_TRACE_EVENT_RETURN_FLAGS_DISABLE_AFTER_PROCESSING;
      }
    }

    test_interface->current_trace_event_ =
        TraceInfo{trace_ref, event_type, module, trace_id};
    return ret;
  }

  // Only adds the event to buffer if the number of bytes inidcates is what is
  // provided.
  static void TraceSinkStartBlock(void* user_data, size_t size) {
    TraceTestInterface* test_interface =
        reinterpret_cast<TraceTestInterface*>(user_data);
    test_interface->sink_block_size_ = size;
    test_interface->sink_bytes_received_ = 0;
  }

  static void TraceSinkAddBytes(void* user_data,
                                const void* bytes,
                                size_t size) {
    TraceTestInterface* test_interface =
        reinterpret_cast<TraceTestInterface*>(user_data);
    static_cast<void>(bytes);
    test_interface->sink_bytes_received_ += size;
  }

  static void TraceSinkEndBlock(void* user_data) {
    TraceTestInterface* test_interface =
        reinterpret_cast<TraceTestInterface*>(user_data);
    if (test_interface->sink_block_size_ ==
        test_interface->sink_bytes_received_) {
      test_interface->buffer_.push_back(test_interface->current_trace_event_);
    }
  }

  // Get the event buffer.
  std::deque<TraceInfo>& GetEvents() { return buffer_; }

  // Check that the next event in the buffer is equal to the expected (and pop
  // that event).
  bool CheckEvent(const TraceInfo& expected) {
    if (buffer_.empty()) {
      return false;
    }
    TraceInfo actual = buffer_.front();
    buffer_.pop_front();
    return actual == expected;
  }

 private:
  ActionOnEvent action_ = ActionOnEvent::None;
  TraceInfo event_match_;
  TraceInfo current_trace_event_;
  size_t sink_block_size_;
  size_t sink_bytes_received_;
  std::deque<TraceInfo> buffer_;
  pw::trace::CallbacksImpl::SinkHandle sink_handle_;
  pw::trace::CallbacksImpl::EventCallbackHandle event_callback_handle_;
};

}  // namespace

// Helper macro to pop the next trace out of test interface and check it against
// expecte values.
#define EXPECT_TRACE(...) PW_DELEGATE_BY_ARG_COUNT(_EXPECT_TRACE, __VA_ARGS__)
#define _EXPECT_TRACE3(interface, event_type, label) \
  _EXPECT_TRACE7(interface,                          \
                 event_type,                         \
                 label,                              \
                 PW_TRACE_GROUP_LABEL_DEFAULT,       \
                 PW_TRACE_TRACE_ID_DEFAULT,          \
                 PW_TRACE_MODULE_NAME,               \
                 PW_TRACE_FLAGS_DEFAULT)
#define _EXPECT_TRACE4(interface, event_type, label, group) \
  _EXPECT_TRACE7(interface,                                 \
                 event_type,                                \
                 label,                                     \
                 group,                                     \
                 PW_TRACE_TRACE_ID_DEFAULT,                 \
                 PW_TRACE_MODULE_NAME,                      \
                 PW_TRACE_FLAGS_DEFAULT)
#define _EXPECT_TRACE5(interface, event_type, label, group, trace_id) \
  _EXPECT_TRACE7(interface,                                           \
                 event_type,                                          \
                 label,                                               \
                 group,                                               \
                 trace_id,                                            \
                 PW_TRACE_MODULE_NAME,                                \
                 PW_TRACE_FLAGS_DEFAULT)
#define _EXPECT_TRACE6(interface, event_type, label, group, trace_id, module) \
  _EXPECT_TRACE7(interface,                                                   \
                 event_type,                                                  \
                 label,                                                       \
                 group,                                                       \
                 trace_id,                                                    \
                 module,                                                      \
                 PW_TRACE_FLAGS_DEFAULT)
#define _EXPECT_TRACE7(                                                      \
    interface, event_type, label, group, trace_id, module, flags)            \
  do {                                                                       \
    static uint32_t _label_token =                                           \
        PW_TRACE_REF(event_type, module, label, flags, group);               \
    EXPECT_TRUE(                                                             \
        interface.CheckEvent({_label_token, event_type, module, trace_id})); \
  } while (0)

#define EXPECT_TRACE_DATA(...) \
  PW_DELEGATE_BY_ARG_COUNT(_EXPECT_TRACE_DATA, __VA_ARGS__)
#define _EXPECT_TRACE_DATA4(interface, event_type, label, data_type) \
  _EXPECT_TRACE_DATA8(interface,                                     \
                      event_type,                                    \
                      label,                                         \
                      PW_TRACE_GROUP_LABEL_DEFAULT,                  \
                      PW_TRACE_TRACE_ID_DEFAULT,                     \
                      data_type,                                     \
                      PW_TRACE_MODULE_NAME,                          \
                      PW_TRACE_FLAGS_DEFAULT)
#define _EXPECT_TRACE_DATA5(interface, event_type, label, group, data_type) \
  _EXPECT_TRACE_DATA8(interface,                                            \
                      event_type,                                           \
                      label,                                                \
                      group,                                                \
                      PW_TRACE_TRACE_ID_DEFAULT,                            \
                      data_type,                                            \
                      PW_TRACE_MODULE_NAME,                                 \
                      PW_TRACE_FLAGS_DEFAULT)
#define _EXPECT_TRACE_DATA6(                                  \
    interface, event_type, label, group, trace_id, data_type) \
  _EXPECT_TRACE_DATA8(interface,                              \
                      event_type,                             \
                      label,                                  \
                      group,                                  \
                      trace_id,                               \
                      data_type,                              \
                      PW_TRACE_MODULE_NAME,                   \
                      PW_TRACE_FLAGS_DEFAULT)
#define _EXPECT_TRACE_DATA7(                                          \
    interface, event_type, label, group, trace_id, data_type, module) \
  _EXPECT_TRACE_DATA8(interface,                                      \
                      event_type,                                     \
                      label,                                          \
                      group,                                          \
                      trace_id,                                       \
                      data_type,                                      \
                      module,                                         \
                      PW_TRACE_FLAGS_DEFAULT)
#define _EXPECT_TRACE_DATA8(                                                   \
    interface, event_type, label, group, trace_id, data_type, module, flags)   \
  do {                                                                         \
    static uint32_t _label_token =                                             \
        PW_TRACE_REF_DATA(event_type, module, label, flags, group, data_type); \
    EXPECT_TRUE(                                                               \
        interface.CheckEvent({_label_token, event_type, module, trace_id}));   \
  } while (0)

// Start of tests

TEST(TokenizedTrace, Instant) {
  TraceTestInterface test_interface;

  PW_TRACE_INSTANT("Test");
  PW_TRACE_INSTANT("Test2", "g");
  PW_TRACE_INSTANT("Test3", "g", 2);

  // Check results
  EXPECT_TRACE(test_interface, PW_TRACE_TYPE_INSTANT, "Test");
  EXPECT_TRACE(test_interface, PW_TRACE_TYPE_INSTANT_GROUP, "Test2", "g");
  EXPECT_TRACE(test_interface, PW_TRACE_TYPE_ASYNC_INSTANT, "Test3", "g", 2);
  EXPECT_TRUE(test_interface.GetEvents().empty());
}

TEST(TokenizedTrace, Duration) {
  TraceTestInterface test_interface;

  PW_TRACE_START("Test");
  PW_TRACE_END("Test");

  // Check results
  EXPECT_TRACE(test_interface, PW_TRACE_TYPE_DURATION_START, "Test");
  EXPECT_TRACE(test_interface, PW_TRACE_TYPE_DURATION_END, "Test");
  EXPECT_TRUE(test_interface.GetEvents().empty());
}

TEST(TokenizedTrace, DurationGroup) {
  TraceTestInterface test_interface;

  PW_TRACE_START("Parent", "group");
  PW_TRACE_START("Child", "group");
  PW_TRACE_END("Child", "group");
  PW_TRACE_END("Parent", "group");

  // Check results
  EXPECT_TRACE(
      test_interface, PW_TRACE_TYPE_DURATION_GROUP_START, "Parent", "group");
  EXPECT_TRACE(
      test_interface, PW_TRACE_TYPE_DURATION_GROUP_START, "Child", "group");
  EXPECT_TRACE(
      test_interface, PW_TRACE_TYPE_DURATION_GROUP_END, "Child", "group");
  EXPECT_TRACE(
      test_interface, PW_TRACE_TYPE_DURATION_GROUP_END, "Parent", "group");
  EXPECT_TRUE(test_interface.GetEvents().empty());
}

TEST(TokenizedTrace, Async) {
  TraceTestInterface test_interface;

  uint32_t trace_id = 1;
  PW_TRACE_START("label for async", "group", trace_id);
  PW_TRACE_INSTANT("label for step", "group", trace_id);
  PW_TRACE_END("label for async", "group", trace_id);

  // Check results
  EXPECT_TRACE(test_interface,
               PW_TRACE_TYPE_ASYNC_START,
               "label for async",
               "group",
               trace_id);
  EXPECT_TRACE(test_interface,
               PW_TRACE_TYPE_ASYNC_INSTANT,
               "label for step",
               "group",
               trace_id);
  EXPECT_TRACE(test_interface,
               PW_TRACE_TYPE_ASYNC_END,
               "label for async",
               "group",
               trace_id);
  EXPECT_TRUE(test_interface.GetEvents().empty());
}

TEST(TokenizedTrace, SkipEvent) {
  TraceTestInterface test_interface;

  // Set trace interface to use skip flag in callback for a specific event.
  TraceTestInterface::TraceInfo skip_event{
      0, PW_TRACE_EVENT_TYPE_INVALID, "", PW_TRACE_TRACE_ID_DEFAULT};
  skip_event.trace_ref = PW_TRACE_REF(PW_TRACE_TYPE_INSTANT,
                                      "TST",
                                      "Test2",
                                      PW_TRACE_FLAGS_DEFAULT,
                                      PW_TRACE_GROUP_LABEL_DEFAULT);
  test_interface.SetCallbackEventAction(TraceTestInterface::ActionOnEvent::Skip,
                                        skip_event);

  PW_TRACE_INSTANT("Test");
  PW_TRACE_INSTANT("Test2");

  // Check results
  EXPECT_TRACE(test_interface, PW_TRACE_TYPE_INSTANT, "Test");
  EXPECT_TRUE(test_interface.GetEvents().empty());
}

TEST(TokenizedTrace, SkipModule) {
  TraceTestInterface test_interface;
  // Set trace interface to use skip flag in callback for a module.
  TraceTestInterface::TraceInfo skip_event{
      0, PW_TRACE_EVENT_TYPE_INVALID, "", PW_TRACE_TRACE_ID_DEFAULT};
  skip_event.module = "SkipModule";
  test_interface.SetCallbackEventAction(TraceTestInterface::ActionOnEvent::Skip,
                                        skip_event);

#undef PW_TRACE_MODULE_NAME
#define PW_TRACE_MODULE_NAME "SkipModule"
  PW_TRACE_INSTANT("Test");
#undef PW_TRACE_MODULE_NAME
#define PW_TRACE_MODULE_NAME "TST"
  PW_TRACE_INSTANT("Test2");

  // Check results
  EXPECT_TRACE(test_interface, PW_TRACE_TYPE_INSTANT, "Test2");
  EXPECT_TRUE(test_interface.GetEvents().empty());
}

TEST(TokenizedTrace, DisableBeforeTrace) {
  TraceTestInterface test_interface;

  // Set trace interface to disable when a specific event happens.
  TraceTestInterface::TraceInfo trigger{
      0, PW_TRACE_EVENT_TYPE_INVALID, "", PW_TRACE_TRACE_ID_DEFAULT};
  // Stop capturing when Test2 event shows up.
  trigger.trace_ref = PW_TRACE_REF(PW_TRACE_TYPE_INSTANT,
                                   "TST",    // Module
                                   "Test2",  // Label
                                   PW_TRACE_FLAGS_DEFAULT,
                                   PW_TRACE_GROUP_LABEL_DEFAULT);
  test_interface.SetCallbackEventAction(
      TraceTestInterface::ActionOnEvent::Disable, trigger);

  PW_TRACE_INSTANT("Test1");
  PW_TRACE_INSTANT("Test2");
  PW_TRACE_INSTANT("Test3");

  // Check results
  EXPECT_TRACE(test_interface, PW_TRACE_TYPE_INSTANT, "Test1");
  EXPECT_TRUE(test_interface.GetEvents().empty());
}

TEST(TokenizedTrace, DisableAfterTrace) {
  TraceTestInterface test_interface;

  // Set trace interface to use flag to disable after a specific event happens.
  TraceTestInterface::TraceInfo trigger{
      0, PW_TRACE_EVENT_TYPE_INVALID, "", PW_TRACE_TRACE_ID_DEFAULT};
  // Stop capturing after Test2 event shows up.
  trigger.trace_ref = PW_TRACE_REF(PW_TRACE_TYPE_INSTANT,
                                   "TST",    // Module
                                   "Test2",  // Label
                                   PW_TRACE_FLAGS_DEFAULT,
                                   PW_TRACE_GROUP_LABEL_DEFAULT);
  test_interface.SetCallbackEventAction(
      TraceTestInterface::ActionOnEvent::DisableAfter, trigger);

  PW_TRACE_INSTANT("Test1");
  PW_TRACE_INSTANT("Test2");
  PW_TRACE_INSTANT("Test3");

  // Check results
  EXPECT_TRACE(test_interface, PW_TRACE_TYPE_INSTANT, "Test1");
  EXPECT_TRACE(test_interface, PW_TRACE_TYPE_INSTANT, "Test2");
  EXPECT_TRUE(test_interface.GetEvents().empty());
}

TEST(TokenizedTrace, Scope) {
  TraceTestInterface test_interface;

  { PW_TRACE_SCOPE("scoped trace"); }

  // Check results
  EXPECT_TRACE(test_interface, PW_TRACE_TYPE_DURATION_START, "scoped trace");
  EXPECT_TRACE(test_interface, PW_TRACE_TYPE_DURATION_END, "scoped trace");
  EXPECT_TRUE(test_interface.GetEvents().empty());
}

TEST(TokenizedTrace, ScopeGroup) {
  TraceTestInterface test_interface;

  { PW_TRACE_SCOPE("scoped group trace", "group"); }

  // Check results
  EXPECT_TRACE(test_interface,
               PW_TRACE_TYPE_DURATION_GROUP_START,
               "scoped group trace",
               "group");
  EXPECT_TRACE(test_interface,
               PW_TRACE_TYPE_DURATION_GROUP_END,
               "scoped group trace",
               "group");
  EXPECT_TRUE(test_interface.GetEvents().empty());
}

TEST(TokenizedTrace, ScopeLoop) {
  TraceTestInterface test_interface;

  for (uint32_t i = 0; i < 10; i++) {
    PW_TRACE_SCOPE("scoped loop", "group", i);
  }
  // Check results
  for (uint32_t i = 0; i < 10; i++) {
    EXPECT_TRACE(
        test_interface, PW_TRACE_TYPE_ASYNC_START, "scoped loop", "group", i);
    EXPECT_TRACE(
        test_interface, PW_TRACE_TYPE_ASYNC_END, "scoped loop", "group", i);
  }
  EXPECT_TRUE(test_interface.GetEvents().empty());
}

TEST(TokenizedTrace, Function) {
  TraceTestInterface test_interface;

  TraceFunction();

  // Check results
  EXPECT_TRACE(
      test_interface,
      PW_TRACE_TYPE_DURATION_START,
      PW_TRACE_FUNCTION_LABEL_FILE_LINE(__FILE__, TRACE_FUNCTION_LINE));
  EXPECT_TRACE(
      test_interface,
      PW_TRACE_TYPE_DURATION_END,
      PW_TRACE_FUNCTION_LABEL_FILE_LINE(__FILE__, TRACE_FUNCTION_LINE));
  EXPECT_TRUE(test_interface.GetEvents().empty());
}

TEST(TokenizedTrace, FunctionGroup) {
  TraceTestInterface test_interface;

  TraceFunctionGroup();

  // Check results
  EXPECT_TRACE(
      test_interface,
      PW_TRACE_TYPE_DURATION_GROUP_START,
      PW_TRACE_FUNCTION_LABEL_FILE_LINE(__FILE__, TRACE_FUNCTION_GROUP_LINE),
      "FunctionGroup");
  EXPECT_TRACE(
      test_interface,
      PW_TRACE_TYPE_DURATION_GROUP_END,
      PW_TRACE_FUNCTION_LABEL_FILE_LINE(__FILE__, TRACE_FUNCTION_GROUP_LINE),
      "FunctionGroup");
  EXPECT_TRUE(test_interface.GetEvents().empty());
}

TEST(TokenizedTrace, FunctionTraceId) {
  TraceTestInterface test_interface;
  static constexpr uint32_t kTraceId = 5;
  TraceFunctionTraceId(kTraceId);

  // Check results
  EXPECT_TRACE(
      test_interface,
      PW_TRACE_TYPE_ASYNC_START,
      PW_TRACE_FUNCTION_LABEL_FILE_LINE(__FILE__, TRACE_FUNCTION_ID_LINE),
      "FunctionGroup",
      kTraceId);
  EXPECT_TRACE(
      test_interface,
      PW_TRACE_TYPE_ASYNC_END,
      PW_TRACE_FUNCTION_LABEL_FILE_LINE(__FILE__, TRACE_FUNCTION_ID_LINE),
      "FunctionGroup",
      kTraceId);
  EXPECT_TRUE(test_interface.GetEvents().empty());
}

TEST(TokenizedTrace, Data) {
  TraceTestInterface test_interface;
  int value = 5;
  PW_TRACE_INSTANT_DATA("label", "i", &value, sizeof(value));
  // Check results
  EXPECT_TRACE_DATA(test_interface,
                    PW_TRACE_TYPE_INSTANT,
                    "label",
                    "i");  // TODO(rgoliver): check data
  EXPECT_TRUE(test_interface.GetEvents().empty());
}

// Create some helper macros that generated some test trace data based from a
// number, and can check that it is correct.
constexpr std::byte kTestData[] = {
    std::byte{0}, std::byte{1}, std::byte{2}, std::byte{3}, std::byte{4}};
#define QUEUE_TESTS_ARGS(num)                               \
  (num), static_cast<pw_trace_EventType>((num) % 10),       \
      "module_" PW_STRINGIFY(num), (num), (num), kTestData, \
      (num) % PW_ARRAY_SIZE(kTestData)
#define QUEUE_CHECK_RESULT(queue_size, result, num)                            \
  result && ((result->trace_token) == (num)) &&                                \
      ((result->event_type) == static_cast<pw_trace_EventType>((num) % 10)) && \
      (strncmp(result->module,                                                 \
               "module_" PW_STRINGIFY(num),                                    \
               strlen("module_" PW_STRINGIFY(num))) == 0) &&                   \
      ((result->trace_id) == (num)) && ((result->flags) == (num)) &&           \
      (memcmp(const_cast<const pw::trace::internal::TraceQueue<                \
                  queue_size>::QueueEventBlock*>(result)                       \
                  ->data_buffer,                                               \
              kTestData,                                                       \
              result->data_size) == 0) &&                                      \
      (result->data_size == (num) % PW_ARRAY_SIZE(kTestData))

TEST(TokenizedTrace, QueueSimple) {
  constexpr size_t kQueueSize = 5;
  pw::trace::internal::TraceQueue<kQueueSize> queue;
  constexpr size_t kTestNum = 1;
  ASSERT_EQ(pw::OkStatus(), queue.TryPushBack(QUEUE_TESTS_ARGS(kTestNum)));
  EXPECT_FALSE(queue.IsEmpty());
  EXPECT_FALSE(queue.IsFull());
  EXPECT_TRUE(QUEUE_CHECK_RESULT(kQueueSize, queue.PeekFront(), kTestNum));
  queue.PopFront();
  EXPECT_TRUE(queue.IsEmpty());
  EXPECT_TRUE(queue.PeekFront() == nullptr);
  EXPECT_FALSE(queue.IsFull());
}

TEST(TokenizedTrace, QueueFull) {
  constexpr size_t kQueueSize = 5;
  pw::trace::internal::TraceQueue<kQueueSize> queue;
  for (size_t i = 0; i < kQueueSize; i++) {
    EXPECT_EQ(queue.TryPushBack(QUEUE_TESTS_ARGS(i)), pw::OkStatus());
  }
  EXPECT_FALSE(queue.IsEmpty());
  EXPECT_TRUE(queue.IsFull());
  EXPECT_EQ(queue.TryPushBack(QUEUE_TESTS_ARGS(1)),
            pw::Status::ResourceExhausted());

  for (size_t i = 0; i < kQueueSize; i++) {
    EXPECT_TRUE(QUEUE_CHECK_RESULT(kQueueSize, queue.PeekFront(), i));
    queue.PopFront();
  }
  EXPECT_TRUE(queue.IsEmpty());
  EXPECT_TRUE(queue.PeekFront() == nullptr);
  EXPECT_FALSE(queue.IsFull());
}

TEST(TokenizedTrace, Clear) {
  constexpr size_t kQueueSize = 5;
  pw::trace::internal::TraceQueue<kQueueSize> queue;
  for (size_t i = 0; i < kQueueSize; i++) {
    EXPECT_EQ(queue.TryPushBack(QUEUE_TESTS_ARGS(i)), pw::OkStatus());
  }
  EXPECT_FALSE(queue.IsEmpty());
  EXPECT_TRUE(queue.IsFull());
  queue.Clear();
  EXPECT_TRUE(queue.IsEmpty());
  EXPECT_TRUE(queue.PeekFront() == nullptr);
  EXPECT_FALSE(queue.IsFull());
}
