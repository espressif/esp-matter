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
"""Does nothing.

The purpose of this script is to allow for source file dependencies within GN
to be attached to targets that do not typically support them, such as groups.

For example, instead of creating a group target, a pw_python_action target to
run this script can be created. The script can be given a list of input files,
causing GN to rebuild the target and everything that depends on it whenever any
input file is modified.

This is useful in the case where metadata is attached to a group of files but
not collected into a generated_file until a later target.
"""
