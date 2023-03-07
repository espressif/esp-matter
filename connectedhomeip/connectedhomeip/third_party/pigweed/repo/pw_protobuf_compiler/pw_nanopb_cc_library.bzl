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
"""WORK IN PROGRESS!

Nanopb C++ library generating targets.
"""

# TODO(b/234873954) Enable unused variable check.
# buildifier: disable=unused-variable
def pw_nanopb_cc_library(
        name,
        deps,
        options = None,
        **kwargs):
    """Generates the nanopb C++ library.

    deps: proto_library targets to convert using nanopb.
    options: Path to the nanopb .options file. See
      https://jpa.kapsi.fi/nanopb/docs/reference.html#proto-file-options
      for the syntax.
    """

    # TODO(tpudlik): Implement this rule. Just a placeholder for now.
    native.cc_library(
        name = name,
    )
