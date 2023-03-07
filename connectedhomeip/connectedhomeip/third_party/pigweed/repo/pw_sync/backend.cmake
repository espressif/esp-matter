# Copyright 2022 The Pigweed Authors
#
# Licensed under the Apache License, Version 2.0 (the "License"); you may not
# use this file except in compliance with the License. You may obtain a copy of
# the License at
#
#     https://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
# WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
# License for the specific language governing permissions and limitations under
# the License.
include_guard(GLOBAL)

include($ENV{PW_ROOT}/pw_build/pigweed.cmake)

# Backend for the pw_sync module's binary semaphore.
pw_add_backend_variable(pw_sync.binary_semaphore_BACKEND)

# Backend for the pw_sync module's condition variable.
pw_add_backend_variable(pw_sync.condition_variable_BACKEND)

# Backend for the pw_sync module's counting semaphore.
pw_add_backend_variable(pw_sync.counting_semaphore_BACKEND)

# Backend for the pw_sync module's mutex.
pw_add_backend_variable(pw_sync.mutex_BACKEND)

# Backend for the pw_sync module's timed mutex.
pw_add_backend_variable(pw_sync.timed_mutex_BACKEND)

# Backend for the pw_sync module's recursive mutex.
pw_add_backend_variable(pw_sync.recursive_mutex_BACKEND)

# Backend for the pw_sync module's interrupt spin lock.
pw_add_backend_variable(pw_sync.interrupt_spin_lock_BACKEND)

# Backend for the pw_sync module's thread notification.
pw_add_backend_variable(pw_sync.thread_notification_BACKEND)

# Backend for the pw_sync module's timed thread notification.
pw_add_backend_variable(pw_sync.timed_thread_notification_BACKEND)
