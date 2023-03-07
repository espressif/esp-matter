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

#include "pw_metric/metric.h"

#include <array>

#include "pw_assert/check.h"
#include "pw_log/log.h"
#include "pw_span/span.h"
#include "pw_tokenizer/base64.h"

namespace pw::metric {
namespace {

template <typename T>
span<const std::byte> AsSpan(const T& t) {
  return span<const std::byte>(reinterpret_cast<const std::byte*>(&t),
                               sizeof(t));
}

// A convenience class to encode a token as base64 while managing the storage.
// TODO(keir): Consider putting this into upstream pw_tokenizer.
struct Base64EncodedToken {
  Base64EncodedToken(Token token) {
    int encoded_size = tokenizer::PrefixedBase64Encode(AsSpan(token), data);
    data[encoded_size] = 0;
  }

  const char* value() { return data.data(); }
  std::array<char, 16> data;
};

const char* Indent(int level) {
  static const char* kWhitespace8 = "        ";
  level = std::min(level, 4);
  return kWhitespace8 + 8 - 2 * level;
}

}  // namespace

// Enable easier registration when used as a member.
Metric::Metric(Token name, float value, IntrusiveList<Metric>& metrics)
    : Metric(name, value) {
  metrics.push_front(*this);
}
Metric::Metric(Token name, uint32_t value, IntrusiveList<Metric>& metrics)
    : Metric(name, value) {
  metrics.push_front(*this);
}

float Metric::as_float() const {
  PW_DCHECK(is_float());
  return float_;
}

uint32_t Metric::as_int() const {
  PW_DCHECK(is_int());
  return uint_;
}

void Metric::Increment(uint32_t amount) {
  PW_DCHECK(is_int());
  uint_ += amount;
}

void Metric::SetInt(uint32_t value) {
  PW_DCHECK(is_int());
  uint_ = value;
}

void Metric::SetFloat(float value) {
  PW_DCHECK(is_float());
  float_ = value;
}

void Metric::Dump(int level) {
  Base64EncodedToken encoded_name(name());
  const char* indent = Indent(level);
  if (is_float()) {
    // Variadic macros promote float to double. Explicitly cast here to
    // acknowledge this and allow projects to use -Wdouble-promotion.
    PW_LOG_INFO("%s \"%s\": %f,",
                indent,
                encoded_name.value(),
                static_cast<double>(as_float()));
  } else {
    PW_LOG_INFO("%s \"%s\": %u,",
                indent,
                encoded_name.value(),
                static_cast<unsigned int>(as_int()));
  }
}

void Metric::Dump(IntrusiveList<Metric>& metrics, int level) {
  for (auto& m : metrics) {
    m.Dump(level);
  }
}

Group::Group(Token name) : name_(name) {}

Group::Group(Token name, IntrusiveList<Group>& groups) : name_(name) {
  groups.push_front(*this);
}

void Group::Dump(int level) {
  Base64EncodedToken encoded_name(name());
  const char* indent = Indent(level);
  PW_LOG_INFO("%s \"%s\": {", indent, encoded_name.value());
  Group::Dump(children(), level + 1);
  Metric::Dump(metrics(), level + 1);
  PW_LOG_INFO("%s }", indent);
}

void Group::Dump(IntrusiveList<Group>& groups, int level) {
  for (auto& group : groups) {
    group.Dump(level);
  }
}

}  // namespace pw::metric
