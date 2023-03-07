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
"""This module defines the string to ID hash used in pw_rpc."""

HASH_CONSTANT = 65599


# This is the same hash function that is used in pw_tokenizer, with the maximum
# length removed. It is chosen due to its simplicity. The tokenizer code is
# duplicated here to avoid unnecessary dependencies between modules.
def hash_65599(string: str) -> int:
    hash_value = len(string)
    coefficient = HASH_CONSTANT

    for char in string:
        hash_value = (hash_value + coefficient * ord(char)) % 2**32
        coefficient = (coefficient * HASH_CONSTANT) % 2**32

    return hash_value


# Function that converts a name to an ID.
calculate = hash_65599
