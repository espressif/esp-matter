#!/usr/bin/env python3
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
"""Tests compiling and importing Python protos on the fly."""

import unittest

from pw_protobuf_compiler_protos import test_pb2 as top_level
from pw_protobuf_compiler_protos.nested.more_nesting import test_pb2


class TestCompileAndImport(unittest.TestCase):
    def test_access_compiled_protobufs(self):
        self.assertNotEqual(top_level.FOO, top_level.BAR)

        message = test_pb2.Message(field=123)
        self.assertEqual(message.field, 123)


if __name__ == '__main__':
    unittest.main()
