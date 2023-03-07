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

#include <array>
#include <cstddef>

namespace {

int volatile* unoptimizable;

template <typename Callable>
class CallableSize {
 public:
  constexpr CallableSize(Callable callable) : callable_(std::move(callable)) {}

  int PreventOptimization() { return *unoptimizable; }

 private:
  alignas(std::max_align_t) Callable callable_;
};

[[maybe_unused]] void Function() {}

class CustomCallableClass {
 public:
  void operator()() {}

 private:
  std::array<std::byte, 16> data_;
};

}  // namespace

int main() {
  int a = 0;
  int b = 1;
  int c = 2;
  int d = 3;
  static_cast<void>(a);
  static_cast<void>(b);
  static_cast<void>(c);
  static_cast<void>(d);

#if defined(_BASE)
  CallableSize<std::array<std::byte, 0>> callable_size({});
#elif defined(_FUNCTION_POINTER)
  static CallableSize callable_size(Function);
#elif defined(_STATIC_LAMBDA)
  static CallableSize callable_size(+[]() {});
#elif defined(_SIMPLE_LAMBDA)
  static CallableSize callable_size([]() {});
#elif defined(_CAPTURING_LAMBDA)
  static CallableSize callable_size([a]() {});
#elif defined(_MULTI_CAPTURING_LAMBDA)
  static CallableSize callable_size([a, b, c, d]() {});
#elif defined(_CUSTOM_CLASS)
  static CallableSize callable_size((CustomCallableClass()));
#endif

  int foo = callable_size.PreventOptimization();
  return sizeof(callable_size) + foo;
}
