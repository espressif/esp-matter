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
    "pw_cc_test",
)

package(default_visibility = ["//visibility:public"])

licenses(["notice"])

pw_cc_library(
    name = "pw_status",
    srcs = [
        "public/pw_status/internal/config.h",
        "status.cc",
    ],
    hdrs = [
        "public/pw_status/status.h",
        "public/pw_status/status_with_size.h",
        "public/pw_status/try.h",
    ],
    includes = ["public"],
)

pw_cc_test(
    name = "status_test",
    srcs = [
        "status_test.cc",
        "status_test_c.c",
    ],
    deps = [
        ":pw_status",
        "//pw_unit_test",
    ],
)

pw_cc_test(
    name = "status_with_size_test",
    srcs = ["status_with_size_test.cc"],
    deps = [
        ":pw_status",
        "//pw_unit_test",
    ],
)

pw_cc_test(
    name = "try_test",
    srcs = ["try_test.cc"],
    deps = [
        ":pw_status",
        "//pw_unit_test",
    ],
)
