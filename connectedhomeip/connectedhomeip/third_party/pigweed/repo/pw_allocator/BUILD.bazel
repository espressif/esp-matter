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
    name = "block",
    srcs = [
        "block.cc",
    ],
    hdrs = [
        "public/pw_allocator/block.h",
    ],
    includes = ["public"],
    deps = [
        "//pw_assert",
        "//pw_span",
        "//pw_status",
    ],
)

pw_cc_library(
    name = "freelist",
    srcs = [
        "freelist.cc",
    ],
    hdrs = [
        "public/pw_allocator/freelist.h",
    ],
    includes = ["public"],
    deps = [
        "//pw_containers",
        "//pw_span",
        "//pw_status",
    ],
)

pw_cc_library(
    name = "freelist_heap",
    srcs = [
        "freelist_heap.cc",
    ],
    hdrs = [
        "public/pw_allocator/freelist_heap.h",
    ],
    includes = ["public"],
    deps = [
        ":block",
        ":freelist",
        "//pw_log",
    ],
)

pw_cc_test(
    name = "block_test",
    srcs = [
        "block_test.cc",
    ],
    deps = [
        ":block",
        "//pw_span",
        "//pw_unit_test",
    ],
)

pw_cc_test(
    name = "freelist_test",
    srcs = [
        "freelist_test.cc",
    ],
    deps = [
        ":freelist",
        "//pw_span",
        "//pw_status",
        "//pw_unit_test",
    ],
)

pw_cc_test(
    name = "freelist_heap_test",
    srcs = [
        "freelist_heap_test.cc",
    ],
    deps = [
        ":freelist_heap",
    ],
)
