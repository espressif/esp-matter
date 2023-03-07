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

#include <cstring>
#include <limits>
#include <string>

#if __cplusplus >= 201703L
#include <string_view>
#endif  // __cplusplus >= 201703L

namespace pw {
namespace kvs {

// Key is a simplified string_view used for KVS. This helps KVS work on
// platforms without C++17.
class Key {
 public:
  using value_type = const char;

  // Constructors
  constexpr Key() : str_{nullptr}, length_{0} {}
  constexpr Key(const Key&) = default;
  constexpr Key(const char* str) : str_{str}, length_{CStringLength(str)} {}
  constexpr Key(const char* str, size_t len) : str_{str}, length_{len} {}
  Key(const std::string& str) : str_{str.data()}, length_{str.length()} {}

#if __cplusplus >= 201703L
  constexpr Key(const std::string_view& str)
      : str_{str.data()}, length_{str.length()} {}
  operator std::string_view() { return std::string_view{str_, length_}; }
#endif  // __cplusplus >= 201703L

  // Assignment
  constexpr Key& operator=(const Key&) = default;

  // Traits
  constexpr size_t size() const { return length_; }
  constexpr size_t length() const { return length_; }
  constexpr bool empty() const { return length_ == 0; }

  // Access
  constexpr const char& operator[](size_t pos) const { return str_[pos]; }
  constexpr const char& at(size_t pos) const { return str_[pos]; }
  constexpr const char& front() const { return str_[0]; }
  constexpr const char& back() const { return str_[length_ - 1]; }
  constexpr const char* data() const { return str_; }

  // Iterator
  constexpr const char* begin() const { return str_; }
  constexpr const char* end() const { return str_ + length_; }

  // Equal
  constexpr bool operator==(Key other_key) const {
    return length() == other_key.length() &&
           std::memcmp(str_, other_key.data(), length()) == 0;
  }

  // Not Equal
  constexpr bool operator!=(Key other_key) const {
    return length() != other_key.length() ||
           std::memcmp(str_, other_key.data(), length()) != 0;
  }

 private:
  // constexpr version of strlen
  static constexpr size_t CStringLength(const char* str) {
    size_t length = 0;
    while (str[length] != '\0') {
      length += 1;
    }
    return length;
  }

  const char* str_;
  size_t length_;
};

}  // namespace kvs
}  // namespace pw
