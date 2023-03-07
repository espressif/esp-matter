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

package(default_visibility = ["//visibility:public"])

licenses(["notice"])

pw_cc_library(
    name = "headers",
    hdrs = [
        "public/pw_log_basic/log_basic.h",
        "public_overrides/pw_log_backend/log_backend.h",
    ],
    includes = [
        "public",
        "public_overrides",
    ],
    deps = [
        "//pw_preprocessor",
    ],
)

pw_cc_library(
    name = "pw_log_basic",
    srcs = [
        "log_basic.cc",
        "pw_log_basic_private/config.h",
    ],
    deps = [
        ":headers",
        "//pw_log:facade",
        "//pw_string",
        "//pw_sys_io",
    ],
)
