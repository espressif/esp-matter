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
    "pw_cc_binary",
)

package(default_visibility = ["//visibility:public"])

licenses(["notice"])

pw_cc_binary(
    name = "single_write_snprintf",
    srcs = ["format_single.cc"],
    copts = ["-DUSE_FORMAT=0"],
    deps = [
        "//pw_bloat:bloat_this_binary",
        "//pw_string",
    ],
)

pw_cc_binary(
    name = "single_write_format",
    srcs = ["format_single.cc"],
    copts = ["-DUSE_FORMAT=1"],
    deps = [
        "//pw_bloat:bloat_this_binary",
        "//pw_string",
    ],
)

pw_cc_binary(
    name = "multiple_writes_snprintf",
    srcs = ["format_multiple.cc"],
    copts = ["-DUSE_FORMAT=0"],
    deps = [
        "//pw_bloat:bloat_this_binary",
        "//pw_string",
    ],
)

pw_cc_binary(
    name = "multiple_writes_format",
    srcs = ["format_multiple.cc"],
    copts = ["-DUSE_FORMAT=1"],
    deps = [
        "//pw_bloat:bloat_this_binary",
        "//pw_string",
    ],
)

pw_cc_binary(
    name = "many_writes_snprintf",
    srcs = ["format_many_without_error_handling.cc"],
    copts = ["-DUSE_FORMAT=0"],
    deps = [
        "//pw_bloat:bloat_this_binary",
        "//pw_string",
    ],
)

pw_cc_binary(
    name = "many_writes_format",
    srcs = ["format_many_without_error_handling.cc"],
    copts = ["-DUSE_FORMAT=1"],
    deps = [
        "//pw_bloat:bloat_this_binary",
        "//pw_string",
    ],
)

pw_cc_binary(
    name = "build_string_with_snprintf_no_base_snprintf",
    srcs = ["string_builder_size_report.cc"],
    copts = [
        "-DUSE_STRING_BUILDER=0",
        "-DPROVIDE_BASE_SNPRINTF=0",
    ],
    deps = [
        "//pw_bloat:bloat_this_binary",
        "//pw_string",
    ],
)

pw_cc_binary(
    name = "build_string_with_string_builder_no_base_snprintf",
    srcs = ["string_builder_size_report.cc"],
    copts = [
        "-DUSE_STRING_BUILDER=1",
        "-DPROVIDE_BASE_SNPRINTF=0",
    ],
    deps = [
        "//pw_bloat:bloat_this_binary",
        "//pw_string",
    ],
)

pw_cc_binary(
    name = "build_string_with_snprintf",
    srcs = ["string_builder_size_report.cc"],
    copts = [
        "-DUSE_STRING_BUILDER=0",
        "-DPROVIDE_BASE_SNPRINTF=1",
    ],
    deps = [
        "//pw_bloat:bloat_this_binary",
        "//pw_string",
    ],
)

pw_cc_binary(
    name = "build_string_with_string_builder",
    srcs = ["string_builder_size_report.cc"],
    copts = [
        "-DUSE_STRING_BUILDER=1",
        "-DPROVIDE_BASE_SNPRINTF=1",
    ],
    deps = [
        "//pw_bloat:bloat_this_binary",
        "//pw_string",
    ],
)

pw_cc_binary(
    name = "build_string_incremental_with_snprintf",
    srcs = ["string_builder_size_report_incremental.cc"],
    copts = ["-DUSE_STRING_BUILDER=0"],
    deps = [
        "//pw_bloat:bloat_this_binary",
        "//pw_string",
    ],
)

pw_cc_binary(
    name = "build_string_incremental_with_string_builder",
    srcs = ["string_builder_size_report_incremental.cc"],
    copts = ["-DUSE_STRING_BUILDER=1"],
    deps = [
        "//pw_bloat:bloat_this_binary",
        "//pw_string",
    ],
)
