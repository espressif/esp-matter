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
    "pw_cc_facade",
    "pw_cc_library",
)

package(default_visibility = ["//visibility:public"])

licenses(["notice"])

pw_cc_facade(
    name = "facade",
    hdrs = [
        "public/pw_malloc/malloc.h",
    ],
    includes = ["public"],
)

pw_cc_library(
    name = "pw_malloc",
    deps = [
        ":facade",
        "@pigweed_config//:pw_malloc_backend",
    ],
)

pw_cc_library(
    name = "backend_multiplexer",
    visibility = ["@pigweed_config//:__pkg__"],
    deps = ["//pw_malloc_freelist"],
)
