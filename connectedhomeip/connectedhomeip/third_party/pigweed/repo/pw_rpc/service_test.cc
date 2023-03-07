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

#include "pw_rpc/service.h"

#include "gtest/gtest.h"
#include "pw_rpc/internal/method.h"

namespace pw::rpc {

class ServiceTestHelper {
 public:
  static const internal::Method* FindMethod(Service& service, uint32_t id) {
    return service.FindMethod(id);
  }
};

namespace {

void InvokeIt(const internal::CallContext&, const internal::Packet&) {}

class ServiceTestMethod : public internal::Method {
 public:
  constexpr ServiceTestMethod(uint32_t id, char the_value)
      : internal::Method(id, InvokeIt), value(the_value) {}

  char value;  // Add a member so the class is larger than the base Method.
};

class ServiceTestMethodUnion : public internal::MethodUnion {
 public:
  constexpr ServiceTestMethodUnion(ServiceTestMethod&& method)
      : impl_({.service_test = method}) {}

  constexpr const internal::Method& method() const { return impl_.method; }

 private:
  union {
    internal::Method method;
    ServiceTestMethod service_test;
  } impl_;
};

class TestService : public Service {
 public:
  constexpr TestService() : Service(0xabcd, kMethods) {}

  static constexpr std::array<ServiceTestMethodUnion, 3> kMethods = {
      ServiceTestMethod(123, 'a'),
      ServiceTestMethod(456, 'b'),
      ServiceTestMethod(789, 'c'),
  };
};

TEST(Service, MultipleMethods_FindMethod_Present) {
  TestService service;
  EXPECT_EQ(ServiceTestHelper::FindMethod(service, 123),
            &TestService::kMethods[0].method());
  EXPECT_EQ(ServiceTestHelper::FindMethod(service, 456),
            &TestService::kMethods[1].method());
  EXPECT_EQ(ServiceTestHelper::FindMethod(service, 789),
            &TestService::kMethods[2].method());
}

TEST(Service, MultipleMethods_FindMethod_NotPresent) {
  TestService service;
  EXPECT_EQ(ServiceTestHelper::FindMethod(service, 0), nullptr);
  EXPECT_EQ(ServiceTestHelper::FindMethod(service, 457), nullptr);
  EXPECT_EQ(ServiceTestHelper::FindMethod(service, 999), nullptr);
}

class EmptyTestService : public Service {
 public:
  constexpr EmptyTestService() : Service(0xabcd, kMethods) {}
  static constexpr std::array<ServiceTestMethodUnion, 0> kMethods = {};
};

TEST(Service, NoMethods_FindMethod_NotPresent) {
  EmptyTestService service;
  EXPECT_EQ(ServiceTestHelper::FindMethod(service, 123), nullptr);
  EXPECT_EQ(ServiceTestHelper::FindMethod(service, 456), nullptr);
  EXPECT_EQ(ServiceTestHelper::FindMethod(service, 789), nullptr);
}

}  // namespace
}  // namespace pw::rpc
