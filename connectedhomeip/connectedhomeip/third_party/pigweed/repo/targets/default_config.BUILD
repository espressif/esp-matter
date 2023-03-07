# Copyright 2021 The Pigweed Authors
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

package(default_visibility = ["//visibility:public"])

# TODO(b/236321905): Support backends other than boringSSL.
label_flag(
    name = "pw_crypto_sha256_backend",
    build_setting_default = "@pigweed//pw_crypto:sha256_boringssl",
)

# TODO(b/236321905): Support backends other than boringSSL.
label_flag(
    name = "pw_crypto_ecdsa_backend",
    build_setting_default = "@pigweed//pw_crypto:ecdsa_boringssl",
)

label_flag(
    name = "pw_log_backend",
    build_setting_default = "@pigweed//pw_log:backend_multiplexer",
)

label_flag(
    name = "pw_log_string_handler_backend",
    build_setting_default = "@pigweed//pw_log_string:handler_backend_multiplexer",
)

label_flag(
    name = "pw_assert_backend",
    build_setting_default = "@pigweed//pw_assert:backend_multiplexer",
)

label_flag(
    name = "pw_boot_backend",
    build_setting_default = "@pigweed//pw_boot:backend_multiplexer",
)

label_flag(
    name = "pw_chrono_system_clock_backend",
    build_setting_default = "@pigweed//pw_chrono:system_clock_backend_multiplexer",
)

label_flag(
    name = "pw_chrono_system_timer_backend",
    build_setting_default = "@pigweed//pw_chrono:system_timer_backend_multiplexer",
)

label_flag(
    name = "pw_rpc_system_server_backend",
    build_setting_default = "@pigweed//pw_rpc/system_server:system_server_backend_multiplexer",
)

label_flag(
    name = "pw_sync_binary_semaphore_backend",
    build_setting_default = "@pigweed//pw_sync:binary_semaphore_backend_multiplexer",
)

label_flag(
    name = "pw_sync_counting_semaphore_backend",
    build_setting_default = "@pigweed//pw_sync:counting_semaphore_backend_multiplexer",
)

label_flag(
    name = "pw_sync_mutex_backend",
    build_setting_default = "@pigweed//pw_sync:mutex_backend_multiplexer",
)

label_flag(
    name = "pw_sync_timed_mutex_backend",
    build_setting_default = "@pigweed//pw_sync:timed_mutex_backend_multiplexer",
)

label_flag(
    name = "pw_sync_recursive_mutex_backend",
    build_setting_default = "@pigweed//pw_sync:recursive_mutex_backend_multiplexer",
)

label_flag(
    name = "pw_sync_interrupt_spin_lock_backend",
    build_setting_default = "@pigweed//pw_sync:interrupt_spin_lock_backend_multiplexer",
)

label_flag(
    name = "pw_sync_thread_notification_backend",
    build_setting_default = "@pigweed//pw_sync:thread_notification_backend_multiplexer",
)

label_flag(
    name = "pw_sync_timed_thread_notification_backend",
    build_setting_default = "@pigweed//pw_sync:timed_thread_notification_backend_multiplexer",
)

label_flag(
    name = "pw_interrupt_backend",
    build_setting_default = "@pigweed//pw_interrupt:backend_multiplexer",
)

label_flag(
    name = "pw_malloc_backend",
    build_setting_default = "@pigweed//pw_malloc:backend_multiplexer",
)

label_flag(
    name = "pw_thread_id_backend",
    build_setting_default = "@pigweed//pw_thread:id_backend_multiplexer",
)

label_flag(
    name = "pw_thread_iteration_backend",
    build_setting_default = "@pigweed//pw_thread:iteration_backend_multiplexer",
)

label_flag(
    name = "pw_thread_sleep_backend",
    build_setting_default = "@pigweed//pw_thread:sleep_backend_multiplexer",
)

label_flag(
    name = "pw_thread_thread_backend",
    build_setting_default = "@pigweed//pw_thread:thread_backend_multiplexer",
)

label_flag(
    name = "pw_thread_yield_backend",
    build_setting_default = "@pigweed//pw_thread:yield_backend_multiplexer",
)

label_flag(
    name = "pw_tokenizer_global_handler_backend",
    build_setting_default = "@pigweed//pw_tokenizer:test_backend",
)

label_flag(
    name = "pw_tokenizer_global_handler_with_payload_backend",
    build_setting_default = "@pigweed//pw_tokenizer:test_backend",
)

label_flag(
    name = "pw_sys_io_backend",
    build_setting_default = "@pigweed//pw_sys_io:backend_multiplexer",
)

label_flag(
    name = "pw_system_target_hooks_backend",
    build_setting_default = "@pigweed//pw_system:target_hooks_multiplexer",
)

label_flag(
    name = "pw_unit_test_googletest_backend",
    build_setting_default = "@pigweed//pw_unit_test:light",
)

label_flag(
    name = "target_rtos",
    build_setting_default = "@pigweed//pw_build/constraints/rtos:none",
)

label_flag(
    name = "pw_perf_test_timer_backend",
    build_setting_default = "@pigweed//pw_perf_test:timer_multiplexer",
)

label_flag(
    name = "pw_trace_backend",
    build_setting_default = "@pigweed//pw_trace:backend_multiplexer",
)
