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
"""A custom wrapper for py_proto_library."""

load("@com_google_protobuf//:protobuf.bzl", real_py_proto_library = "py_proto_library")

def py_proto_library(**kwargs):
    """A py_proto_library that respects the "manual" tag.

    Actually it's a little _too_ respectful: it simply removes those targets
    from the BUILD graph. This is good enough for our use case. Please do not
    use this outside upstream Pigweed. (For downstream projects, this is
    equivalent to just commenting out the BUILD target.)

    See https://issues.pigweed.dev/issues/244743459 and
    https://stackoverflow.com/q/74323506/1224002 for more context.
    """
    if "tags" not in kwargs or "manual" not in kwargs["tags"]:
        real_py_proto_library(**kwargs)
