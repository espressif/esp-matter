# Copyright 2019 The Pigweed Authors
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
    name = "pw_sys_io_stdio",
    srcs = ["sys_io.cc"],
    target_compatible_with = select(TARGET_COMPATIBLE_WITH_HOST_SELECT),
    deps = [
        "//pw_sys_io:default_putget_bytes",
        "//pw_sys_io:facade",
    ],
)
