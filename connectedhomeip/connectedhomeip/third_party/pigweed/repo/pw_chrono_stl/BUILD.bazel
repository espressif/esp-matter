# Copyright 2020 The Pigweed Authors
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

load(
    "//pw_build:pigweed.bzl",
    "pw_cc_library",
)
load(
    "//pw_build:selects.bzl",
    "TARGET_COMPATIBLE_WITH_HOST_SELECT",
)

package(default_visibility = ["//visibility:public"])

licenses(["notice"])

pw_cc_library(
    name = "system_clock_headers",
    hdrs = [
        "public/pw_chrono_stl/system_clock_config.h",
        "public/pw_chrono_stl/system_clock_inline.h",
        "public_overrides/pw_chrono_backend/system_clock_config.h",
        "public_overrides/pw_chrono_backend/system_clock_inline.h",
    ],
    includes = [
        "public",
        "public_overrides",
    ],
    deps = [
        "//pw_chrono:epoch",
    ],
)

pw_cc_library(
    name = "system_clock",
    target_compatible_with = select(TARGET_COMPATIBLE_WITH_HOST_SELECT),
    deps = [
        ":system_clock_headers",
        "//pw_chrono:system_clock_facade",
    ],
)

pw_cc_library(
    name = "system_timer_headers",
    hdrs = [
        "public/pw_chrono_stl/system_timer_inline.h",
        "public/pw_chrono_stl/system_timer_native.h",
        "public_overrides/pw_chrono_backend/system_timer_inline.h",
        "public_overrides/pw_chrono_backend/system_timer_native.h",
    ],
    includes = [
        "public",
        "public_overrides",
    ],
    deps = [
        "//pw_chrono:system_clock",
        "//pw_chrono:system_timer_facade",
        "//pw_function",
    ],
)

pw_cc_library(
    name = "system_timer",
    srcs = [
        "system_timer.cc",
    ],
    target_compatible_with = select(TARGET_COMPATIBLE_WITH_HOST_SELECT),
    deps = [
        ":system_timer_headers",
        "//pw_chrono:system_timer_facade",
    ],
)
