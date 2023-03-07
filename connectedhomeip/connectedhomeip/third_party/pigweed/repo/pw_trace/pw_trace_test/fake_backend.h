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

#pragma once

// Enable traces
#define PW_TRACE_TYPE_INSTANT trace_fake_backend::Instantaneous
#define PW_TRACE_TYPE_INSTANT_GROUP trace_fake_backend::InstantaneousGroup
#define PW_TRACE_TYPE_DURATION_START trace_fake_backend::DurationStart
#define PW_TRACE_TYPE_DURATION_END trace_fake_backend::DurationEnd

#define PW_TRACE_TYPE_DURATION_GROUP_START \
  trace_fake_backend::DurationGroupStart
#define PW_TRACE_TYPE_DURATION_GROUP_END trace_fake_backend::DurationGroupEnd

#define PW_TRACE_TYPE_ASYNC_START trace_fake_backend::AsyncStart
#define PW_TRACE_TYPE_ASYNC_INSTANT trace_fake_backend::AsyncStep
#define PW_TRACE_TYPE_ASYNC_END trace_fake_backend::AsyncEnd

namespace trace_fake_backend {

typedef enum {
  Invalid,
  Instantaneous,
  InstantaneousGroup,
  AsyncStart,
  AsyncStep,
  AsyncEnd,
  DurationStart,
  DurationEnd,
  DurationGroupStart,
  DurationGroupEnd,
} pw_trace_EventType;

// Define a helper class for holding events and checking equality.
class Event {
 public:
  Event()
      : event_type_(Invalid),
        flags_(0),
        label_(nullptr),
        group_(nullptr),
        trace_id_(0),
        has_data_(false),
        data_format_string_(nullptr),
        data_(nullptr),
        data_size_(0) {}
  Event(pw_trace_EventType event_type,
        uint8_t flags,
        const char* label,
        const char* group,
        uint32_t trace_id)
      : event_type_(event_type),
        flags_(flags),
        label_(label),
        group_(group),
        trace_id_(trace_id),
        has_data_(false),
        data_format_string_(nullptr),
        data_(nullptr),
        data_size_(0) {}
  Event(pw_trace_EventType event_type,
        uint8_t flags,
        const char* label,
        const char* group,
        uint32_t trace_id,
        const char* data_type,
        const char* data,
        size_t size)
      : event_type_(event_type),
        flags_(flags),
        label_(label),
        group_(group),
        trace_id_(trace_id),
        has_data_(true),
        data_format_string_(data_type),
        data_(data),
        data_size_(size) {}
  bool operator==(const Event& rhs) const {
    return event_type_ == rhs.event_type_ &&                      //
           flags_ == rhs.flags_ &&                                //
           label_ == rhs.label_ &&                                //
           group_ == rhs.group_ && trace_id_ == rhs.trace_id_ &&  //
           has_data_ == rhs.has_data_ &&                          //
           data_format_string_ == rhs.data_format_string_ &&      //
           data_size_ == rhs.data_size_ &&                        //
           (data_size_ == 0 ||                                    //
            (memcmp(data_, rhs.data_, data_size_) == 0));
  }

  bool IsEqualIgnoreLabel(const Event& rhs) const {
    return event_type_ == rhs.event_type_ &&                      //
           flags_ == rhs.flags_ &&                                //
           group_ == rhs.group_ && trace_id_ == rhs.trace_id_ &&  //
           has_data_ == rhs.has_data_ &&                          //
           data_format_string_ == rhs.data_format_string_ &&      //
           data_size_ == rhs.data_size_ &&                        //
           (data_size_ == 0 ||                                    //
            (memcmp(data_, rhs.data_, data_size_) == 0));
  }

 private:
  pw_trace_EventType event_type_;
  uint8_t flags_;
  const char* label_;
  const char* group_;
  uint32_t trace_id_;

  bool has_data_;
  const char* data_format_string_;
  const char* data_;
  size_t data_size_;
};

class LastEvent {
 public:
  static LastEvent& Instance() { return instance_; }
  Event& Get() { return last_event_; }
  void Set(const Event& event) { last_event_ = event; }

 private:
  Event last_event_{};
  static LastEvent instance_;
};

#define PW_TRACE(event_type, flags, label, group, trace_id) \
  LastEvent::Instance().Set(Event(event_type, flags, label, group, trace_id));

#define PW_TRACE_DATA(                                           \
    event_type, flags, label, group, trace_id, type, data, size) \
  LastEvent::Instance().Set(                                     \
      Event(event_type, flags, label, group, trace_id, type, data, size));

}  // namespace trace_fake_backend
