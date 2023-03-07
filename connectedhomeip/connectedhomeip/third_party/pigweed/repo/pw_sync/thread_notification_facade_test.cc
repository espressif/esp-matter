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

#include <chrono>

#include "gtest/gtest.h"
#include "pw_sync/thread_notification.h"

namespace pw::sync {
namespace {

TEST(ThreadNotification, EmptyInitialState) {
  ThreadNotification notification;
  EXPECT_FALSE(notification.try_acquire());
}

// TODO(b/235284163): Add real concurrency tests.

TEST(ThreadNotification, Release) {
  ThreadNotification notification;
  notification.release();
  notification.release();
  notification.acquire();
  // Ensure it fails when empty.
  EXPECT_FALSE(notification.try_acquire());
}

ThreadNotification empty_initial_notification;
TEST(ThreadNotification, EmptyInitialStateStatic) {
  EXPECT_FALSE(empty_initial_notification.try_acquire());
}

ThreadNotification raise_notification;
TEST(ThreadNotification, ReleaseStatic) {
  raise_notification.release();
  raise_notification.release();
  raise_notification.acquire();
  // Ensure it fails when empty.
  EXPECT_FALSE(raise_notification.try_acquire());
}

}  // namespace
}  // namespace pw::sync
