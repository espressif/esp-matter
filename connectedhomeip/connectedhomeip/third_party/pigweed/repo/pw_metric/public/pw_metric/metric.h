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

#include <algorithm>
#include <initializer_list>
#include <limits>

#include "pw_containers/intrusive_list.h"
#include "pw_preprocessor/arguments.h"
#include "pw_tokenizer/tokenize.h"

namespace pw::metric {

// Currently, this is for tokens, but later may be a char* when non-tokenized
// metric names are supported.
using tokenizer::Token;

#define _PW_METRIC_TOKEN_MASK 0x7fffffff

// An individual metric. There are only two supported types: uint32_t and
// float. More complicated compound metrics can be built on these primitives.
// See the documentation for a discussion for this design was selected.
//
// Size: 12 bytes / 96 bits - next, name, value.
//
// TODO(keir): Implement Set() and Increment() using atomics.
// TODO(keir): Consider an alternative structure where metrics have pointers to
// parent groups, which would enable (1) safe destruction and (2) safe static
// initialization, but at the cost of an additional 4 bytes per metric and 4
// bytes per group..
class Metric : public IntrusiveList<Metric>::Item {
 public:
  Token name() const { return name_and_type_ & kTokenMask; }

  bool is_float() const { return (name_and_type_ & kTypeMask) == kTypeFloat; }
  bool is_int() const { return (name_and_type_ & kTypeMask) == kTypeInt; }

  float as_float() const;
  uint32_t as_int() const;

  // Dump a metric or metrics to logs. Level determines the indentation
  // indent_level up to a maximum of 4. Example output:
  //
  //   "$FCM4qQ==": 0,
  //
  // Note the base64-encoded token name. Detokenization tools are necessary to
  // convert this to human-readable form.
  void Dump(int indent_level = 0);
  static void Dump(IntrusiveList<Metric>& metrics, int indent_level = 0);

  // Disallow copy and assign.
  Metric(Metric const&) = delete;
  void operator=(const Metric&) = delete;

 protected:
  Metric(Token name, float value)
      : name_and_type_((name & kTokenMask) | kTypeFloat), float_(value) {}

  Metric(Token name, uint32_t value)
      : name_and_type_((name & kTokenMask) | kTypeInt), uint_(value) {}

  Metric(Token name, float value, IntrusiveList<Metric>& metrics);
  Metric(Token name, uint32_t value, IntrusiveList<Metric>& metrics);

  // Hide mutation methods, and only offer write access through the specialized
  // TypedMetric below. This makes it impossible to call metric.Increment() on
  // a float metric at compile time.
  void Increment(uint32_t amount = 1);

  void SetInt(uint32_t value);

  void SetFloat(float value);

 private:
  // The name of this metric as a token; from PW_TOKENIZE_STRING("my_metric").
  // Last bit of the token is used to store int or float; 0 == int, 1 == float.
  Token name_and_type_;

  union {
    float float_;
    uint32_t uint_;
  };

  enum : uint32_t {
    kTokenMask = _PW_METRIC_TOKEN_MASK,  // 0x7fff'ffff
    kTypeMask = 0x8000'0000,
    kTypeFloat = 0x8000'0000,
    kTypeInt = 0x0,
  };
};

// TypedMetric provides a type-safe wrapper the runtime-typed Metric object.
// Note: Definition omitted to prevent accidental instantiation.
// TODO(keir): Provide a more precise error message via static assert.
template <typename T>
class TypedMetric;

// A metric for floats. Does not offer an Increment() function, since it is too
// easy to do unsafe operations like accumulating small values in floats.
template <>
class TypedMetric<float> : public Metric {
 public:
  TypedMetric(Token name, float value) : Metric(name, value) {}
  TypedMetric(Token name, float value, IntrusiveList<Metric>& metrics)
      : Metric(name, value, metrics) {}

  void Set(float value) { SetFloat(value); }
  float value() const { return Metric::as_float(); }

 private:
  // Shadow these accessors to hide them on the typed version of Metric.
  float as_float() const { return 0.0; }
  uint32_t as_int() const { return 0; }
};

// A metric for uint32_ts. Offers both Set() and Increment().
template <>
class TypedMetric<uint32_t> : public Metric {
 public:
  TypedMetric(Token name, uint32_t value) : Metric(name, value) {}
  TypedMetric(Token name, uint32_t value, IntrusiveList<Metric>& metrics)
      : Metric(name, value, metrics) {}

  void Increment(uint32_t amount = 1u) { Metric::Increment(amount); }
  void Set(uint32_t value) { SetInt(value); }
  uint32_t value() const { return Metric::as_int(); }

 private:
  // Shadow these accessors to hide them on the typed version of Metric.
  float as_float() const { return 0.0; }
  uint32_t as_int() const { return 0; }
};

// A metric tree; consisting of children groups and leaf metrics.
//
// Size: 16 bytes/128 bits - next, name, metrics, children.
class Group : public IntrusiveList<Group>::Item {
 public:
  Group(Token name);
  Group(Token name, IntrusiveList<Group>& groups);

  Token name() const { return name_; }

  void Add(Metric& metric) { metrics_.push_front(metric); }
  void Add(Group& group) { children_.push_front(group); }

  IntrusiveList<Metric>& metrics() { return metrics_; }
  IntrusiveList<Group>& children() { return children_; }

  const IntrusiveList<Metric>& metrics() const { return metrics_; }
  const IntrusiveList<Group>& children() const { return children_; }

  // Dump a metric group or groups to logs. Level determines the indentation
  // indent_level up to a maximum of 4. Example output:
  //
  //   "$6doqFw==": {
  //     "$05OCZw==": {
  //       "$VpPfzg==": 1,
  //       "$LGPMBQ==": 1.000000,
  //       "$+iJvUg==": 5,
  //     }
  //     "$9hPNxw==": 65,
  //     "$oK7HmA==": 13,
  //     "$FCM4qQ==": 0,
  //   }
  //
  // Note the base64-encoded token name. Detokenization tools are necessary to
  // convert this to human-readable form.
  void Dump(int indent_level = 0);
  static void Dump(IntrusiveList<Group>& groups, int indent_level = 0);

  // Disallow copy and assign.
  Group(Group const&) = delete;
  void operator=(const Group&) = delete;

 private:
  // The name of this group as a token; from PW_TOKENIZE_STRING("my_group").
  Token name_;

  IntrusiveList<Metric> metrics_;
  IntrusiveList<Group> children_;
};

// Declare a metric, optionally adding it to a group. Use:
//
//   PW_METRIC(variable_name, metric_name, value)
//   PW_METRIC(group, variable_name, metric_name, value)
//
// - variable_name is an identifier
// - metric_name is a string name for the metric (will be tokenized)
// - value must be either a floating point value (3.2f) or unsigned int (21u).
// - group is a Group instance.
//
// The macro declares a variable or member named "name" with type Metric, and
// works in three contexts: global, local, and member.
//
// 1. At global scope
//
//    PW_METRIC(foo, 15.5f);
//
//    void MyFunc() {
//      foo.Increment();
//    }
//
// 2. At local function or member function scope:
//
//    void MyFunc() {
//      PW_METRIC(foo, "foo", 15.5f);
//      foo.Increment();
//      // foo goes out of scope here; be careful!
//    }
//
// 3. At member level inside a class or struct:
//
//    struct MyStructy {
//      void DoSomething() {
//        somethings_.Increment();
//      }
//      // Every instance of MyStructy will have a separate somethings counter.
//      PW_METRIC(somethings_, "somethings", 0u);
//    }
//
// You can also put a metric into a group with the macro. Metrics can belong to
// strictly one group, otherwise a assertion will fail. Example:
//
//   PW_METRIC_GROUP(my_group, "my_group_name_here");
//   PW_METRIC(my_group, foo_, "foo", 0.2f);
//   PW_METRIC(my_group, bar_, "bar", 44000u);
//   PW_METRIC(my_group, zap_, "zap", 3.14f);
//
// NOTE: If you want a globally registered metric, see pw_metric/global.h; in
// that contexts, metrics are globally registered without the need to centrally
// register in a single place.
#define PW_METRIC(...) PW_DELEGATE_BY_ARG_COUNT(_PW_METRIC_, , __VA_ARGS__)
#define PW_METRIC_STATIC(...) \
  PW_DELEGATE_BY_ARG_COUNT(_PW_METRIC_, static, __VA_ARGS__)

// Force conversion to uint32_t for non-float types, no matter what the
// platform uses as the "u" suffix literal. This enables dispatching to the
// correct TypedMetric specialization.
#define _PW_METRIC_FLOAT_OR_UINT32(literal)                       \
  std::conditional_t<std::is_floating_point_v<decltype(literal)>, \
                     float,                                       \
                     uint32_t>

// Case: PW_METRIC(name, initial_value)
#define _PW_METRIC_4(static_def, variable_name, metric_name, init)            \
  static constexpr uint32_t variable_name##_token =                           \
      PW_TOKENIZE_STRING_MASK("metrics", _PW_METRIC_TOKEN_MASK, metric_name); \
  static_def ::pw::metric::TypedMetric<_PW_METRIC_FLOAT_OR_UINT32(init)>      \
      variable_name = {variable_name##_token, init}

// Case: PW_METRIC(group, name, initial_value)
#define _PW_METRIC_5(static_def, group, variable_name, metric_name, init)     \
  static constexpr uint32_t variable_name##_token =                           \
      PW_TOKENIZE_STRING_MASK("metrics", _PW_METRIC_TOKEN_MASK, metric_name); \
  static_def ::pw::metric::TypedMetric<_PW_METRIC_FLOAT_OR_UINT32(init)>      \
      variable_name = {variable_name##_token, init, group.metrics()}

// Define a metric group. Works like PW_METRIC, and works in the same contexts.
//
// Example:
//
//   class MySubsystem {
//    public:
//     void DoSomething() {
//       attempts.Increment();
//       if (ActionSucceeds()) {
//         successes.Increment();
//       }
//     }
//     const Group& metrics() const { return metrics_; }
//     Group& metrics() { return metrics_; }
//
//    private:
//     PW_METRIC_GROUP(metrics_, "my_subsystem");
//     PW_METRIC(metrics_, attempts_, "attempts", 0u);
//     PW_METRIC(metrics_, successes_, "successes", 0u);
//   };
//
#define PW_METRIC_GROUP(...) \
  PW_DELEGATE_BY_ARG_COUNT(_PW_METRIC_GROUP_, , __VA_ARGS__)
#define PW_METRIC_GROUP_STATIC(...) \
  PW_DELEGATE_BY_ARG_COUNT(_PW_METRIC_GROUP_, static, __VA_ARGS__)

#define _PW_METRIC_GROUP_3(static_def, variable_name, group_name) \
  static constexpr uint32_t variable_name##_token =               \
      PW_TOKENIZE_STRING_DOMAIN("metrics", group_name);           \
  static_def ::pw::metric::Group variable_name = {variable_name##_token}

#define _PW_METRIC_GROUP_4(static_def, parent, variable_name, group_name) \
  static constexpr uint32_t variable_name##_token =                       \
      PW_TOKENIZE_STRING_DOMAIN("metrics", group_name);                   \
  static_def ::pw::metric::Group variable_name = {variable_name##_token,  \
                                                  parent.children()}

}  // namespace pw::metric
